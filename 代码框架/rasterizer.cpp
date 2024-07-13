// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const Vector3f* _v)
{   

    float abx=_v[1].x()-_v[0].x();
    float bcx=_v[2].x()-_v[1].x();
    float cax=_v[0].x()-_v[2].x();
    float apx=x-_v[0].x();
    float bpx=x-_v[1].x();
    float cpx=x-_v[2].x();
    float aby=_v[1].y()-_v[0].y();
    float bcy=_v[2].y()-_v[1].y();
    float cay=_v[0].y()-_v[2].y();
    float apy=y-_v[0].y();
    float bpy=y-_v[1].y();
    float cpy=y-_v[2].y();
    float result1=abx*apy-apx*aby;
    float result2=bcx*bpy-bpx*bcy;
    float result3=cax*cpy-cay*cpx;
    if(result1>=0&&result2>=0&&result3>=0)
        return true;
    else if(result1<=0&&result2<=0&&result3<=0)
        return true;
    else
        return false;
    
}


static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    
    float count[4][2]={{-0.25,-0.25},{-0.25,0.25},{0.25,-0.25},{0.25,0.25}};
    auto v = t.toVector4();
    float rightBound = std::max(std::max(t.v[0].x(),t.v[1].x()),std::max(t.v[1].x(),t.v[2].x()));
    float leftBound = std::min(std::min(t.v[0].x(),t.v[1].x()),std::min(t.v[1].x(),t.v[2].x()));
    float topBound = std::max(std::max(t.v[0].y(),t.v[1].y()),std::max(t.v[1].y(),t.v[2].y()));
    float bottomBound = std::min(std::min(t.v[0].y(),t.v[1].y()),std::min(t.v[1].y(),t.v[2].y()));
    for(int x=leftBound;x<=rightBound;x++)
        for(int y=bottomBound;y<=topBound;y++)
        {
            int number=0;
            for(int c=0;c<4;c++)
                if(insideTriangle(x+count[c][0],y+count[c][1],t.v))
                    {
                        auto[alpha, beta, gamma] = computeBarycentric2D(x+count[c][0], y+count[c][1], t.v);
                        float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                        float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                        z_interpolated *= w_reciprocal;
                        if(-z_interpolated<super_depth_buf[get_super_index(x*2+c%2,y*2+c/2)])
                        {
                            number++;
                            super_depth_buf[get_super_index(x*2+c%2,y*2+c/2)]=-z_interpolated;
                            super_frame_buf[get_super_index(x*2+c%2,y*2+c/2)]=t.getColor();
                        }
                    }
            if(number>0)
            {
                set_pixel({x,y,1},(super_frame_buf[get_super_index(x * 2, y * 2)] +
                                         super_frame_buf[get_super_index(x * 2 + 1, y * 2)] +
                                         super_frame_buf[get_super_index(x * 2, y * 2 + 1)] +
                                         super_frame_buf[get_super_index(x * 2 + 1, y * 2 + 1)]) / 4.0f);
            }
        }
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        std::fill(super_frame_buf.begin(), super_frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        std::fill(super_depth_buf.begin(), super_depth_buf.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    super_depth_buf.resize(w*h*4);
    super_frame_buf.resize(w*h*4);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

int rst::rasterizer::get_super_index(int x, int y)
{
    return (height*2-1-y)*width*2 + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on