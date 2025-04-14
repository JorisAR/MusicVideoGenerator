#ifndef UTILS_H
#define UTILS_H

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <vector>

namespace godot
{

class Utils : public Object
{
    GDCLASS(Utils, Object);

  public:
    Utils()
    {
    }
    ~Utils()
    {
    }

    static PackedFloat32Array vector_to_array_float(const std::vector<float> &vec)
    {
        PackedFloat32Array array;
        array.resize(vec.size());
        std::copy(vec.begin(), vec.end(), array.ptrw());
        return array;
    }

    static std::vector<float> array_to_vector_float(const PackedFloat32Array &array)
    {
        std::vector<float> vec(array.size());
        std::copy(array.ptr(), array.ptr() + array.size(), vec.begin());
        return vec;
    }

    static PackedInt32Array vector_to_array_int(const std::vector<int> &vec)
    {
        PackedInt32Array array;
        array.resize(vec.size());
        std::copy(vec.begin(), vec.end(), array.ptrw());
        return array;
    }

    static std::vector<int> array_to_vector_int(const PackedInt32Array &array)
    {
        std::vector<int> vec(array.size());
        std::copy(array.ptr(), array.ptr() + array.size(), vec.begin());
        return vec;
    }

    static godot::String vector_to_string_float(const std::vector<float> &vec)
    {
        godot::String string = "[";
        for (const float &f : vec)
        {
            string += String::num(f, 2) + ", ";
        }
        return string + "]";
    }

    // template <class matType> static godot::String to_string(const matType &v)
    // {
    //     return godot::String(("{" + glm::to_string(v) + "}").c_str());
    // }

    static void print_projection(Projection projection) {
        String str = "Projection:\n";
        for (int i = 0; i < 4; i++)
        {
            str += String::num(projection[i].x) + ", " + String::num(projection[i].y) + ", " + String::num(projection[i].z) + ", " + String::num(projection[i].w) + "\n";
        }
        UtilityFunctions::print(str);
    }

    static inline void projection_to_float(float *target, const Projection &t)
    {
        for (size_t i = 0; i < 4; i++)
        {
            target[i * 4] = t.columns[i].x;
            target[i * 4 + 1] = t.columns[i].y;
            target[i * 4 + 2] = t.columns[i].z;
            target[i * 4 + 3] = t.columns[i].w;
        }
    }

    static inline void projection_to_float_transposed(float *target, const Projection &t)
    {
        for (size_t i = 0; i < 4; i++)
        {
            target[i * 4] = t.columns[0][i];
            target[i * 4 + 1] = t.columns[1][i];
            target[i * 4 + 2] = t.columns[2][i];
            target[i * 4 + 3] = t.columns[3][i];
        }
        
    }
};

} // namespace godot

#endif
