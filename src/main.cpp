#include <omp.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cstdio>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "bmp.hpp"
#include "bezier.hpp"
#include "ray.hpp"
#include "render_pt.hpp"
#include "render_ppm.hpp"
#include "material.hpp"
#include "hit.hpp"
#include "kdtree.hpp"
#include "light.hpp"

#include <string>
#include <array>
#include <plane.hpp>
#include <cstdlib>
#include <ctime>

/*
    先从摄像机看一遍每个像素，找到所有漫反射点
    把这些漫反射点建一棵kd树
    光源一轮轮发射光子，光子打到物体表面就计算对像素点的亮度值贡献
*/

using namespace std;

SceneParser *sceneParser;
Camera *cam;
Image *image;
Group *baseGroup;
int imgW, imgH;
unsigned short X[3];
bool debug = false;

const double eps = 1e-6;
const int MAX_DEP = 6;

void pt(){
    const int PT_SAMP = 200;
    int limx = cam->getWidth();
    int limy = cam->getHeight();
    #pragma omp parallel for schedule(dynamic, 1)
    for (int x = 0; x < limx; ++ x){
        for (int y = 0; y < limy; ++y){
            Vector3f res = Vector3f::ZERO;
            for (int sx = 0; sx < 2; ++sx)
                for (int sy = 0; sy < 2; ++sy){
                    double r1 = 2 * erand48(X), dx = r1<1?sqrt(r1):2-sqrt(2-r1);
                    double r2 = 2 * erand48(X), dy = r2<1?sqrt(r2):2-sqrt(2-r2);
                    Ray camRay = cam->generateJitterRay(Vector2f(x + dx/2 + sx, y + dy/2 + sy));
                    X[0] = y + x + sx;
                    X[1] = y * x + y + sy + time(0);
                    X[2] = y * x * x + y + x + sx * sy + time(0);

                    Vector3f col = Vector3f::ZERO;
                    for (int k = 0; k < PT_SAMP; ++k){
                        Vector3f tmpcol = getColor(camRay, 0, baseGroup);
                        col = col + tmpcol;
                    }
                    res = res + col / PT_SAMP / 4;
                }
            image->SetPixel(x, y, res);
        }
        printf("Finish %d / %d\n", x, cam->getWidth());
    }
}

void ppm(int argc, char *argv[]){
    int ITER = atoi(argv[4]);
    double radius = atof(argv[5]);
    double alpha = atof(argv[6]);

    int thread_count = omp_get_num_procs();
    int limx = cam->getWidth();
    int limy = cam->getHeight();
    int samp_count = atoi(argv[7]) * limx * limy;
    PixelColor **c = new PixelColor*[thread_count];
    for (int i = 0; i < thread_count; ++i)
        c[i] = new PixelColor[limx * limy];
    PixelColor *total = new PixelColor[limx * limy];
    PixelColor *temp = new PixelColor[limx * limy];

    std::vector<SPPMNode> *photon = new std::vector<SPPMNode>[thread_count];

    int sight_point_count = 0;
    int pixel_count = limx * limy;

    for (int iter = 0; iter < ITER; ++iter){
        printf("Iteration %5d start...\n", iter);
        if (iter > 0){
            samp_count /= sqrt(alpha);
            radius *= alpha;
        }
        #pragma omp parallel for num_threads(thread_count) schedule(dynamic, 1)
        for (int x = 0; x < limx; ++ x){
            int thread_num = omp_get_thread_num();
            for (int y = 0; y < limy; ++y){
                for (int sx = 0; sx < 2; ++sx)
                    for (int sy = 0; sy < 2; ++sy){
                        double r1 = 2 * erand48(X), dx = r1<1?sqrt(r1):2-sqrt(2-r1);
                        double r2 = 2 * erand48(X), dy = r2<1?sqrt(r2):2-sqrt(2-r2);
                        Ray camRay = cam->generateJitterRay(Vector2f(x + dx/2 + sx, y + dy/2 + sy));
                        X[0] = y + x + sx;
                        X[1] = y * x + y + sy + time(0);
                        X[2] = y * x * x + y + x + sx * sy + time(0);
                        std::vector<SPPMNode> tmp_sightpoint; 
                        camera_pass(camRay, 0, y * limx + x, tmp_sightpoint, radius, baseGroup);
                        for (int i = 0, lim = tmp_sightpoint.size(); i < lim; ++i)
                            if (tmp_sightpoint[i].index >= 0){  // 视线的这个点可以在屏幕上显示
                                tmp_sightpoint[i].radius = radius;
                                photon[thread_num].push_back(tmp_sightpoint[i]);
                                ++sight_point_count;
                            }
                    }
            }
            if (thread_num == 0)
                printf("Complete %d / %d\n", x, cam->getWidth());
        }
        printf("Iteration %5d collect sight points complete. \nTotal sight points = %d\nTotal Pixels = %d\n", iter, sight_point_count, pixel_count);

        std::vector<SPPMNode> tmpPhoton;
        for (int i = 0; i < thread_count; ++i)
            tmpPhoton.insert(tmpPhoton.end(), photon[i].begin(), photon[i].end());
        KDTree tree(tmpPhoton);
        printf("Iteration %5d build kdtree complete.\n", iter);

        int samp_per_thread = samp_count / thread_count + 1;
        printf("Samp per thread %d\n", samp_per_thread);
        #pragma omp parallel for num_threads(thread_count) schedule(dynamic, 1)
        for (int th = 0; th < thread_count; ++th){
            X[0] = th;
            X[1] = th * th;
            X[2] = iter + th * th + time(0);
            int thread_num = omp_get_thread_num();
            for (int i = 0; i < samp_per_thread; ++i){
                if (thread_num == 0 && i % 1000 == 0)
                    printf("Iteration %5d sppm tracing %5.2f%%\n", iter, 100.0 * (i+1) / samp_per_thread);
                for (int j = 0; j < sceneParser->getNumLights(); ++j){
                    Light *light = sceneParser->getLight(j);
                    Vector3f d = Vector3f::randomRay(Vector3f(0,0,1));
                    if (erand48(X) < 0.5) d = -d;

                    tree.query(SPPMNode(light->getPosition(), light->getColor(), d), c[th]);
                    light_pass(Ray(light->getPosition(), d), 0, light->getColor(), c[th], tree, baseGroup);
                }
            }
        }

        memset(temp, 0, sizeof temp);
        for (int i = 0; i < thread_count; ++i){
            for (int j = 0; j < limx * limy; ++j)
                temp[j] = temp[j] + c[i][j];
            memset(c[i], 0, sizeof c[i]);
        }
        printf("Iteration %5d generate photon complete.\n", iter);

        for (int i = 0; i < limx * limy; ++i){
            total[i] = total[i] + temp[i];
            image->SetPixel(i % limx, i / limx, total[i].getColor());
        }
        image->SaveBMP(argv[2]);
        printf("Iteration %5d save picture complete.\n", iter);
    }

    delete[] photon;
    delete[] total;
    delete[] temp;
    for (int i = 0; i < thread_count; ++i)
        delete[] c[i];
    delete[] c;
}

const char args[][20] = {"Prog", "Input Filename", "Output Filename", "Render Mode", "Render Iterations", "Radius", "Alpha", "Samples Per Pixel"};

int main(int argc, char *argv[]) {

    time_t tbegin, tend;
    time(&tbegin);
    for (int argNum = 1; argNum < argc; ++argNum) {
        std::cout << "Argument " << args[argNum] << " is: " << argv[argNum] << std::endl;
    }

    if (argc < 4 || (strcmp(argv[3], "pt") != 0 && strcmp(argv[3], "ppm") != 0)) {
        cout << "Usage: Proj <input scene file> <output image file> <pt/ppm>" << endl;
        return 1;
    }

    sceneParser = new SceneParser(argv[1]);
    cam = sceneParser->getCamera();
    image = new Image(cam->getWidth(), cam->getHeight());
    baseGroup = sceneParser->getGroup();

    if (strcmp(argv[3], "pt") == 0) pt();
    else ppm(argc, argv);

    image->SaveBMP(argv[2]);
    time(&tend);
    printf("Total time = %d s\n", tend - tbegin);
    delete image;

    return 0;
}

