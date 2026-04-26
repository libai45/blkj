#ifndef WORLD_TO_SCREEN_H
#define WORLD_TO_SCREEN_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float m[4][4];
} Mat4;

/**
 * @brief 设置视图投影矩阵（viewProjection = projection * view）
 */
void w2s_set_view_projection_matrix(const Mat4 *vp);

/**
 * @brief 设置屏幕尺寸（像素）
 */
void w2s_set_viewport(float width, float height);

/**
 * @brief 世界坐标 -> 屏幕坐标
 *
 * @param x 世界坐标 x
 * @param y 世界坐标 y
 * @param z 世界坐标 z
 * @param screenX 输出屏幕 x（像素）
 * @param screenY 输出屏幕 y（像素，左上角为原点）
 * @return true 投影成功（点在近平面前方），false 失败
 */
bool world_to_screen(float x, float y, float z, float *screenX, float *screenY);

/**
 * @brief 矩阵乘法 out = a * b
 */
Mat4 mat4_mul(Mat4 a, Mat4 b);

/**
 * @brief 生成透视投影矩阵（右手坐标，NDC z: [-1, 1]）
 */
Mat4 mat4_perspective(float fovYRadians, float aspect, float zNear, float zFar);

/**
 * @brief 生成观察矩阵（LookAt）
 */
Mat4 mat4_look_at(float eyeX, float eyeY, float eyeZ,
                  float centerX, float centerY, float centerZ,
                  float upX, float upY, float upZ);

#ifdef __cplusplus
}
#endif

#endif // WORLD_TO_SCREEN_H
