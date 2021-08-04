/* * * * * * * * * * * * * * * * * * * * *
*   File:     vector.hpp
*   Author:   Li Kunyun
*   group:    CDCS-HPC
*   Time:     2021-07-28
* * * * * * * * * * * * * * * * * * * * * */

#ifndef __CHIPSUM_NUMERIC_VECTOR_HPP__
#define __CHIPSUM_NUMERIC_VECTOR_HPP__


#include <fstream>

#include "numeric_traits.hpp"
#include "impl/vector_serial_impl.hpp"
#include "impl/vector_kokkoskernels_impl.hpp"





namespace ChipSum {
namespace Numeric {

template<typename ...Props>
class Vector;

template<typename ScalarType,typename SizeType,typename BackendType,typename ...Props>
class Vector<ScalarType,SizeType,BackendType,Props...>{

public:


    using traits = Vector_Traits<ScalarType,SizeType,BackendType,Props...>;


    using vector_type = typename traits::vector_type;


    using size_type = typename traits::size_type;
    using const_size_type = typename traits::const_size_type;
    using size_type_reference = typename std::add_lvalue_reference<size_type>::type;
    using const_size_type_reference = typename std::add_const<size_type_reference>::type;

    using vector_type_reference = typename std::add_lvalue_reference<vector_type>::type;
    using const_vector_type_reference = typename std::add_const<vector_type_reference>::type;


    using device_scalar_type = typename traits::device_scalar_value_type;
    using device_scalar_type_reference = typename std::add_lvalue_reference<device_scalar_type>::type;;

private:

    vector_type __data;
    size_type __size;

private:
//    friend class DenseMatrix<ScalarType,SizeType,BackendType,Props...>;

    //    void privateSample(){}

protected:

    //    void _protectedSample(){}

public:

    //    void PublicSample(){}


    /**
     * @brief Vector：构造函数（创建一个未初始化的Vector）
     * @param size：向量维度
     */
    CHIPSUM_DECLARED_FUNCTION Vector() = default;



    /**
     * @brief Vector：构造函数（创建一个未初始化的Vector）
     * @param size：向量维度
     */
    CHIPSUM_DECLARED_FUNCTION Vector(const_size_type size):
        __size(size)
    {
        ChipSum::Numeric::Impl::Vector::Create<ScalarType,SizeType>(__size,__data);
    }

    /**
     * @brief Vector：构造函数
     * @param data：向量数据抽象类
     * @param size：向量维度
     */
    CHIPSUM_DECLARED_FUNCTION Vector(const vector_type& data,const_size_type size):
        __size(size)
    {
        ChipSum::Numeric::Impl::Vector::Create<ScalarType,SizeType>(__size,__data);
        ChipSum::Numeric::Impl::Vector::DeepCopy<ScalarType,SizeType>(__data,data);
    }





    /**
     * @brief Vector：构造函数
     * @param data：
     * @param size
     */
    CHIPSUM_DECLARED_FUNCTION Vector(typename traits::nonconst_scalar_type* data,const_size_type size)
        :__size(size)
    {

        ChipSum::Numeric::Impl::Vector::Create<ScalarType,SizeType>(data,size,__data);
    }



    /**
     * @brief GetData：获取向量数据
     * @return 向量数据
     */

    CHIPSUM_FUNCTION_INLINE const_vector_type_reference GetData(){return __data;}


    /**
     * @brief GetSize：获取向量维度
     * @return 向量维度
     */
    CHIPSUM_FUNCTION_INLINE const_size_type_reference GetSize(){return __size;}


    /**
     * @brief Dot：向量内积操作
     * @param v：右端项（向量）
     * @param r：内积结果
     */

    CHIPSUM_FUNCTION_INLINE ScalarType Dot(Vector& v){
        double r;
        ChipSum::Numeric::Impl::Vector::
                Dot<ScalarType,SizeType>(GetData(),v.GetData(),__size,r);
        return r;
    }


    /**
     * @brief Dot：向量内积操作（未测试）
     * @param v：右端项（向量）
     * @param r：内积结果（设备）
     */

    template<typename Args>
    CHIPSUM_FUNCTION_INLINE void Dot(Vector& v,Args& r){
        ChipSum::Numeric::Impl::Vector::
                Dot<ScalarType,SizeType>(GetData(),v.GetData(),__size,r);
    }

    /**
     * @brief operator *：向量乘矢量（等比缩放）
     * @param s：
     * @return
     */
    CHIPSUM_FUNCTION_INLINE Vector operator*(typename traits::const_scalar_type s){

        Vector ret(__data,__size);
        ChipSum::Numeric::Impl::Vector::Scal<ScalarType,SizeType>
                (ret.GetData(),s,GetData());

        return ret;
    }

    /**
     * @brief operator *=
     * @param s
     * @return
     */
    CHIPSUM_FUNCTION_INLINE Vector& operator*=(ScalarType s){
        ChipSum::Numeric::Impl::Vector::Scal<ScalarType,SizeType>
                (__data,s,GetData());
        return *this;
    }

    /**
     * @brief operator +
     * @param s
     * @return
     */
    CHIPSUM_FUNCTION_INLINE Vector operator+(Vector& s){
        Vector ret(__data,__size);
        ChipSum::Numeric::Impl::Vector::Axpy<ScalarType,SizeType>
                (static_cast<ScalarType>(1),s.GetData(),ret.GetData());

        return ret;

    }

    /**
     * @brief operator +=
     * @param s
     * @return
     */
    CHIPSUM_FUNCTION_INLINE Vector& operator+=(Vector& s){
        ChipSum::Numeric::Impl::Vector::Axpy<ScalarType,SizeType>
                (static_cast<ScalarType>(1),s.GetData(),__data);
        return *this;
    }

    /**
     * @brief operator -
     * @param s
     * @return
     */
    CHIPSUM_FUNCTION_INLINE Vector operator-(Vector& s){

        Vector ret(__data,__size);
        ChipSum::Numeric::Impl::Vector::Axpy<ScalarType,SizeType>
                (static_cast<ScalarType>(-1),s.GetData(),ret.GetData());
        return ret;
    }

    /**
     * @brief operator -
     * @param s
     * @return
     */
    CHIPSUM_FUNCTION_INLINE Vector operator-(){



        return static_cast<ScalarType>(-1)*(*this);
    }

    /**
     * @brief operator -=
     * @param s
     * @return
     */
    CHIPSUM_FUNCTION_INLINE Vector& operator-=(Vector& s){
        ChipSum::Numeric::Impl::Vector::Axpy<ScalarType,SizeType>
                (static_cast<ScalarType>(-1),s.GetData(),__data);
        return *this;
    }


    CHIPSUM_FUNCTION_INLINE ScalarType& operator()(const SizeType i){
        return ChipSum::Numeric::Impl::Vector::GetItem<ScalarType,SizeType>(i,__data);
    }





    /**
     * @brief Norm1
     * @return
     */
    CHIPSUM_FUNCTION_INLINE ScalarType Norm1(){
        return ChipSum::Numeric::Impl::Vector::Norm1<ScalarType,SizeType,Props...>(__data);
    }


    /**
     * @brief Norm2
     * @return
     */
    CHIPSUM_FUNCTION_INLINE ScalarType Norm2(){
        return ChipSum::Numeric::Impl::Vector::Norm2<ScalarType,SizeType,Props...>(__data);
    }


    CHIPSUM_FUNCTION_INLINE void Print(std::ostream& out=std::cout){
        ChipSum::Numeric::Impl::Vector::Print<ScalarType,SizeType,Props...>(out,__data);
    }


};




template<typename ScalarType,typename SizeType,typename BackendType,typename ...Props>
CHIPSUM_FUNCTION_INLINE Vector<ScalarType,SizeType,BackendType,Props...>
operator*(ScalarType s,Vector<ScalarType,SizeType,BackendType,Props...>& v){
    return v*s;
}



} // End namespace Numeric
} // End namespace ChipSum



typedef ChipSum::Numeric::Vector<double,std::size_t,ChipSum::Backend::DefaultBackend> Vector;
typedef ChipSum::Numeric::Vector<double,std::size_t,ChipSum::Backend::Serial> SerialVector;


#endif // VECTOR_HPP
