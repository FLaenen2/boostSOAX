//
// Created by Francois Laenen on 01/07/2016.
//

#ifndef BOOSTCOMPUTE_ITERATORS_H
#define BOOSTCOMPUTE_ITERATORS_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <boost/compute/algorithm/transform.hpp>
#include <boost/compute/container/vector.hpp>
#include <boost/compute/lambda.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/compute/system.hpp>
#include <boost/compute/functional/math.hpp>
#include <unistd.h>
#include <boost/range/algorithm/copy.hpp>
#include <boost/compute/buffer.hpp>
#include <boost/compute/buffer.hpp>

#undef PRINTOP

template<typename Container, typename iterator_tag = std::random_access_iterator_tag>
class Iterator_facaded : public boost::iterator_facade<Iterator_facaded<Container>, typename Container::value_type, iterator_tag, ::boost::compute::detail::buffer_value<typename Container::value_type>>{
//class Iterator_facaded : public boost::iterator_facade<Iterator_facaded<T>, T, std::random_access_iterator_tag>{ // doesn't work

public:
    using T = typename Container::value_type;
    //using T = typename Container::value_type;
    using super_type = boost::iterator_facade<Iterator_facaded<T>, T, std::random_access_iterator_tag, boost::compute::detail::buffer_value<T>>;
    //using Container = compute::vector<T>;
    // using reference = const compute::detail::buffer_value<T>;
    //using reference = T&; // will not work  (non-const lvalue reference to type 'float' cannot bind to a temporary of type 'reference' (aka 'buffer_value<float>'))
    //using reference = compute::detail::device_ptr<T>;
    using reference = typename super_type::reference;
    //using reference = typename Container::reference;
    using difference_type = typename super_type::difference_type;

    Iterator_facaded(Container &vec) : i_(0), vptr(&vec){}
    Iterator_facaded(Container &vec, int start) : i_(start), vptr(&vec){}

private:

    void increment() {
#ifdef PRINTOP
        printf("void Iterator_facaded::increment(void)  i %d\n", i_);
#endif
        i_++;
    }

    bool equal(Iterator_facaded const& other) const {
#ifdef PRINTOP
        printf("bool Iterator_facaded::increment(Iterator_facaded const&) const  i %d\n", i_);
#endif
        return this->i_ == other.i_;
    }

    reference dereference() const {
    //typename Container::value_type& dereference() const {
#ifdef PRINTOP
        printf("bool Iterator_facaded::dereference(void)  i %d\n", i_);
#endif
        return (*vptr)[i_];
    }


    void decrement(){
    #ifdef PRINTOP
        printf("void Iterator_facaded::decrement(void)  i %d\n", i_);
#endif
        i_--;
    };

    void advance(int n){
    #ifdef PRINTOP
        printf("void Iterator_facaded::advance(int)  i %d\n", i_);
#endif
        i_+=n;
    };

    int get_index(void) const{
    #ifdef PRINTOP
        printf("int Iterator_facaded::get_index(void)  i %d\n", i_);
#endif
        return i_;
    }

    int distance_to(const Iterator_facaded<T> &other) const{
    #ifdef PRINTOP
        printf("int Iterator_facaded::distance_to(const Iterator_facaded<T> &other)  i %d\n", i_);
#endif
        return this->i_ - other.i_;
    }

    friend class boost::iterator_core_access;
    int i_;
    Container *vptr;
};//*/




template<typename T, typename iterator_tag = std::random_access_iterator_tag>
class Iterator_facaded_buff : public boost::iterator_facade<Iterator_facaded_buff<T>, T, iterator_tag, ::boost::compute::detail::buffer_value<T>>{

public:
    using Container = boost::compute::vector<T>;
    using super_type = boost::iterator_facade<Iterator_facaded_buff<T>, T, std::random_access_iterator_tag, boost::compute::detail::buffer_value<T>>;
    using reference = typename super_type::reference;
    using difference_type = typename super_type::difference_type;

    Iterator_facaded_buff(Container &vec) : m_index(0), m_buffer(vec.get_buffer().get(), false){}
    Iterator_facaded_buff(Container &vec, int start) : m_index(start), m_buffer(vec.get_buffer().get(), false){}

    ~Iterator_facaded_buff(){m_buffer.get() = 0;}

    size_t get_index(void) const{
#ifdef PRINTOP
        printf("int Iterator_facaded::get_index(void)  i %d\n", m_index);
#endif
        return m_index;
    }

    const boost::compute::buffer& get_buffer() const
    {
        return m_buffer;
    }

    template<class Expr>
    boost::compute::detail::buffer_iterator_index_expr<T, Expr>
    operator[](const Expr &expr) const
    {
        BOOST_ASSERT(m_buffer.get());

        return boost::compute::detail::buffer_iterator_index_expr<T, Expr>(
                m_buffer, m_index, boost::compute::memory_object::global_memory, expr
        );
    }

private:
    void increment() {
#ifdef PRINTOP
        printf("void Iterator_facaded::increment(void)  i %d\n", m_index);
#endif
        m_index++;
    }

    bool equal(Iterator_facaded_buff const& other) const {
#ifdef PRINTOP
        printf("bool Iterator_facaded::increment(Iterator_facaded const&) const  i %d\n", m_index);
#endif
        return this->m_index == other.m_index && m_buffer.get() == other.m_buffer.get();
    }

    reference dereference() const {
#ifdef PRINTOP
        printf("reference Iterator_facaded::dereference(void)  i %d %g\n", m_index, boost::compute::detail::buffer_value<T>(m_buffer, m_index * sizeof(T)));
#endif
        return boost::compute::detail::buffer_value<T>(m_buffer, m_index * sizeof(T));
    }

    void decrement(){
#ifdef PRINTOP
        printf("void Iterator_facaded::decrement(void)  i %d\n", m_index);
#endif
        m_index--;
    };

    void advance(int n){
#ifdef PRINTOP
        printf("void Iterator_facaded::advance(int)  i %d\n", m_index);
#endif
        m_index = static_cast<size_t>(static_cast<difference_type>(m_index) + n);
    };

    int distance_to(const Iterator_facaded_buff<T> &other) const{
#ifdef PRINTOP
        printf("int Iterator_facaded::distance_to(const Iterator_facaded<T> &other)  i %d\n", m_index);
#endif
        return static_cast<difference_type>(this->m_index - other.m_index);
    }

    friend class boost::iterator_core_access;
    size_t m_index;
    //Container *vptr;
    const boost::compute::buffer m_buffer;
};//*/



template<typename Container, typename iterator_tag = std::random_access_iterator_tag>
class CIterator_facaded : public boost::iterator_facade<CIterator_facaded<Container>, typename Container::value_type, iterator_tag, ::boost::compute::detail::buffer_value<typename Container::value_type>>{
public:
    using T = typename Container::value_type;
    using difference_type = std::ptrdiff_t;
    using reference = ::boost::compute::detail::buffer_value<T>;
    //using reference = float&; // will not work (trying to bind float& to buffer_value<float>)
    CIterator_facaded(Container &vec) : i_(0), vptr(&vec){}
    CIterator_facaded(Container &vec, difference_type start) : i_(start), vptr(&vec){}

    void increment() {
    #ifdef PRINTOP
        printf("void CIterator_facaded::increment(void)  i %ld\n", i_);
#endif
        i_++;
    }
    bool equal(CIterator_facaded const& other) const {
    #ifdef PRINTOP
        printf("bool CIterator_facaded::increment(CIterator_facaded const&) const  i %ld\n", i_);
#endif
        return this->i_ == other.i_;
    }
    reference dereference() const {
    #ifdef PRINTOP
        printf("reference CIterator_facaded::dereference(void)  i %ld\n", i_);
#endif
        return (*vptr)[i_];
    }
    //float& dereference() const { return (*vptr)[i_]; }
    void decrement(){
    #ifdef PRINTOP
        printf("void CIterator_facaded::decrement(void)  i %ld\n", i_);
#endif
        i_--;
    };
    void advance(int n){
    #ifdef PRINTOP
        printf("void CIterator_facaded::advance(int)  i %ld\n", i_);
#endif
        i_+=n;
    };
    int get_index(void) const{
    #ifdef PRINTOP
        printf("int CIterator_facaded::get_index(void)  i %ld\n", i_);
#endif
        return i_;
    }
    int distance_to(const CIterator_facaded<T> &other) const{
    #ifdef PRINTOP
        printf("int CIterator_facaded::distance_to(const CIterator_facaded<T> &other)  i %ld\n", i_);
#endif
        return this->i_ - other.i_;
    }
    difference_type i_;

private :
    friend class boost::iterator_core_access;

    const Container *vptr;
};//*/


template<typename Container, typename iterator_tag = std::random_access_iterator_tag>
class IteratorFL : public std::iterator<iterator_tag, typename Container::value_type>
//class IteratorFL : public boost::iterator<input_iterator_tag, typename Container::value_type>
{
public:
    IteratorFL(Container &myvec, int i) : i_(i), ptr_(&myvec) {};
    bool operator==(const IteratorFL& that) const {
        printf("bool operator==(const IteratorFL& i %d\n", i_);
        //    std::cerr << "  bool operator==(const Iterator& that) const : " << bool(i_ == that.i_) << std::endl;
        return i_ == that.i_;
    }
    IteratorFL& operator++() {
        printf("  IteratorFL& operator++() i %d\n", i_);
        //    std::cerr << "  Iterator& operator++()\n";
        ++i_; return *this;
    }
    bool operator!=(const IteratorFL& that) const {
        printf("bool operator!=(const IteratorFL& i %d\n", i_);
        //    std::cerr << "  bool operator!=(const Iterator& that) const : " << !(*this == that) << std::endl;
        return !( i_ == that.i_);
    }//*/
    const typename Container::reference operator*() const{
    //void operator*(){
    //this->reference operator*() const{
        printf("IteratorFL operator*()  i %d\n", i_);
        //std::cerr << "thrust::device_reference<float> operator*() : " << i_ << " value " << (*ptr_)[i_] << std::endl;
        return (*ptr_)[i_];
    }

private:
    Container *ptr_;
    int i_;
};

template<typename Container, typename iterator_tag = std::random_access_iterator_tag>
class CIteratorFL : public std::iterator<iterator_tag, typename Container::value_type>
//class IteratorFL : public boost::iterator<input_iterator_tag, typename Container::value_type>
{
public:
    CIteratorFL(Container &myvec, int i) : i_(i), ptr_(&myvec) {};
    bool operator==(const CIteratorFL& that) const {
        printf("bool operator==(const CIteratorFL&  i %d\n", i_);
        //    std::cerr << "  bool operator==(const Iterator& that) const : " << bool(i_ == that.i_) << std::endl;
        return i_ == that.i_;
    }
    CIteratorFL& operator++() {
        printf("CIteratorFL& operator++() i %d\n", i_);
        //    std::cerr << "  Iterator& operator++()\n";
        ++i_; return *this;
    }
    bool operator!=(const CIteratorFL& that) const {
        printf("bool operator!=(const CIteratorFL& i %d\n", i_);
        //    std::cerr << "  bool operator!=(const Iterator& that) const : " << !(*this == that) << std::endl;
        return !( i_ == that.i_);
    }//*/
    const typename Container::reference operator*() const{
        //this->reference operator*() const{
        printf("CIteratorFL operator*() i %d\n", i_);
        //std::cerr << "thrust::device_reference<float> operator*() : " << i_ << " value " << (*ptr_)[i_] << std::endl;
        return (*ptr_)[i_];
    }

private:
    Container *ptr_;
    int i_;
};


#endif //BOOSTCOMPUTE_ITERATORS_H
