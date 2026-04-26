#include "world_to_screen.h"

#include <float.h>
#include <math.h>
#include <stddef.h>

static Mat4 g_view_proj = {
    .m = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }
};

static float g_viewport_w = 1170.0f;
static float g_viewport_h = 2532.0f;

static float vec3_dot(float ax, float ay, float az, float bx, float by, float bz) {
    return ax * bx + ay * by + az * bz;
}

static void vec3_cross(float ax, float ay, float az,
                       float bx, float by, float bz,
                       float *rx, float *ry, float *rz) {
    *rx = ay * bz - az * by;
    *ry = az * bx - ax * bz;
    *rz = ax * by - ay * bx;
}

static void vec3_normalize(float *x, float *y, float *z) {
    float len = sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    if (len > FLT_EPSILON) {
        *x /= len;
        *y /= len;
        *z /= len;
    }
}

void w2s_set_view_projection_matrix(const Mat4 *vp) {
    if (!vp) {
        return;
    }
    g_view_proj = *vp;
}

void w2s_set_viewport(float width, float height) {
    if (width > 0.0f) {
        g_viewport_w = width;
    }
    if (height > 0.0f) {
        g_viewport_h = height;
    }
}

Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 out = {0};
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            out.m[r][c] = a.m[r][0] * b.m[0][c]
                        + a.m[r][1] * b.m[1][c]
                        + a.m[r][2] * b.m[2][c]
                        + a.m[r][3] * b.m[3][c];
        }
    }
    return out;
}

Mat4 mat4_perspective(float fovYRadians, float aspect, float zNear, float zFar) {
    Mat4 out = {0};

    float f = 1.0f / tanf(fovYRadians * 0.5f);
    float nf = 1.0f / (zNear - zFar);

    out.m[0][0] = f / aspect;
    out.m[1][1] = f;
    out.m[2][2] = (zFar + zNear) * nf;
    out.m[2][3] = (2.0f * zFar * zNear) * nf;
    out.m[3][2] = -1.0f;

    return out;
}

Mat4 mat4_look_at(float eyeX, float eyeY, float eyeZ,
                  float centerX, float centerY, float centerZ,
                  float upX, float upY, float upZ) {
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;
    vec3_normalize(&fx, &fy, &fz);

    float upnx = upX, upny = upY, upnz = upZ;
    vec3_normalize(&upnx, &upny, &upnz);

    float sx, sy, sz;
    vec3_cross(fx, fy, fz, upnx, upny, upnz, &sx, &sy, &sz);
    vec3_normalize(&sx, &sy, &sz);

    float ux, uy, uz;
    vec3_cross(sx, sy, sz, fx, fy, fz, &ux, &uy, &uz);

    Mat4 out = {0};

    out.m[0][0] = sx;
    out.m[0][1] = sy;
    out.m[0][2] = sz;
    out.m[0][3] = -vec3_dot(sx, sy, sz, eyeX, eyeY, eyeZ);

    out.m[1][0] = ux;
    out.m[1][1] = uy;
    out.m[1][2] = uz;
    out.m[1][3] = -vec3_dot(ux, uy, uz, eyeX, eyeY, eyeZ);

    out.m[2][0] = -fx;
    out.m[2][1] = -fy;
    out.m[2][2] = -fz;
    out.m[2][3] = vec3_dot(fx, fy, fz, eyeX, eyeY, eyeZ);

    out.m[3][3] = 1.0f;

    return out;
}

bool world_to_screen(float x, float y, float z, float *screenX, float *screenY) {
    if (!screenX || !screenY) {
        return false;
    }

    // clip = VP * [x y z 1]^T
    float clipX = g_view_proj.m[0][0] * x + g_view_proj.m[0][1] * y + g_view_proj.m[0][2] * z + g_view_proj.m[0][3];
    float clipY = g_view_proj.m[1][0] * x + g_view_proj.m[1][1] * y + g_view_proj.m[1][2] * z + g_view_proj.m[1][3];
    float clipZ = g_view_proj.m[2][0] * x + g_view_proj.m[2][1] * y + g_view_proj.m[2][2] * z + g_view_proj.m[2][3];
    float clipW = g_view_proj.m[3][0] * x + g_view_proj.m[3][1] * y + g_view_proj.m[3][2] * z + g_view_proj.m[3][3];

    (void)clipZ;

    // 在相机后方或接近近平面，认为不可投影
    if (clipW <= 0.0001f) {
        return false;
    }

    // 透视除法 => NDC [-1,1]
    float ndcX = clipX / clipW;
    float ndcY = clipY / clipW;

    // NDC -> 屏幕坐标（左上角原点）
    *screenX = (ndcX + 1.0f) * 0.5f * g_viewport_w;
    *screenY = (1.0f - ndcY) * 0.5f * g_viewport_h;

    return true;
}
