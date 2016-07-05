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
#include <boost/compute/type_traits/is_device_iterator.hpp>

#include "iterators.h"

namespace compute = boost::compute;
using namespace std;

template<typename T>
class Iterator
{
public:
    using difference_type = std::ptrdiff_t;
    using value_type = float;
    using pointer = float*;
    using reference = float&;
    using iterator_category = std::random_access_iterator_tag;

    Iterator(compute::vector<float> &df, int i) : ptr_(&df), i_(i) {
        printf("In Iterator constructor \n");
    }

    Iterator(compute::vector<float> *df, int i) : i_(i) {
        printf("Iterator(thrust::device_vector<float> *df, int i)\n");
    }

    typename boost::compute::vector<T>::reference operator*() {
        printf("thrust::device_reference<float> operator*()\n");
        //std::cerr << "thrust::device_reference<float> operator*() : " << i_ << " value " << (*ptr_)[i_] << std::endl;
        return (*ptr_)[i_];
    }

    bool operator!=(const Iterator& that) const {
        printf("bool operator!=(const Iterator& that)\n");
        //    std::cerr << "  bool operator!=(const Iterator& that) const : " << !(*this == that) << std::endl;
        return !( i_ == that.i_);
    }

    bool operator==(const Iterator& that) const {
        printf("bool operator==(const Iterator& that)\n");
        //    std::cerr << "  bool operator==(const Iterator& that) const : " << bool(i_ == that.i_) << std::endl;
        return i_ == that.i_;
    }

    Iterator& operator++() {
        printf("  Iterator& operator++()\n");
        //    std::cerr << "  Iterator& operator++()\n";
        ++i_; return *this;
    }

    compute::vector<T> *ptr_;
    int i_;
};

template<class AW_Type>
Iterator<AW_Type> beg(AW_Type&& aw)
{
    return Iterator<AW_Type>(std::forward<AW_Type>(aw),0);
}
template<class AW_Type>
Iterator<AW_Type> end(AW_Type&& aw)
{
    return Iterator<AW_Type>(std::forward<AW_Type>(aw),aw.size());
}

/*template<class AW_Type>
CIterator<AW_Type> cbeg(AW_Type&& aw)
{
    return CIterator<AW_Type>(std::forward<AW_Type>(aw),0);
}


template<class AW_Type>
CIterator<AW_Type> cend(AW_Type&& aw)
{
    return CIterator<AW_Type>(std::forward<AW_Type>(aw),aw.size());
}

}//*/ // namespace SOAX

class Test_BCompute{
public:
    Test_BCompute() {
        cout << "construct" << endl;
        // get default device and setup context
        //device = compute::system::default_device();
        //compute::context context = compute::context(device);
        //queue = compute::command_queue(context, device);
        //device_vector = compute::vector<float>(10000);
        device_vector.resize(10000);
    }
    void sleep(unsigned seconds)
    {
        usleep(seconds * 1000);
    }
    void createDevVec(){
        cout << "createDevVec" << endl;
        // generate random data on the host
        host_vector.resize(10000);
        //std::generate(host_vector.begin(), host_vector.end(), rand);
        std::fill(host_vector.begin(), host_vector.end(), 1);

        // transfer data from the host to the device
        //compute::copy( host_vector.begin(), host_vector.end(), device_vector.begin());//*/
       // printf("filling device vector of size %d\n", device_vector.size());
        compute::command_queue queue = compute::system::default_queue();
        compute::fill(device_vector.begin(), device_vector.end(), 3., queue);

        sleep(2);

        queue.enqueue_barrier();
        cout << device_vector[1] << endl;
        cout << host_vector[1] << endl;

    }

    void transform(){
        cout << "transform" << endl;
        // calculate the square-root of each element in-place
        using boost::compute::lambda::_1;
        compute::command_queue queue = compute::system::default_queue();
        compute::transform(
                device_vector.begin(),
                device_vector.end(),
                device_vector.begin(),
                _1+4
        );
        using boost::compute::fill;
        compute::fill(device_vector.begin(), device_vector.end(), 5., queue);
        compute::fill(device_vector.begin(), device_vector.end(), 6., queue);
        queue.enqueue_barrier();

        //cout << device_vector[1] << endl;
        // copy values back to the host
        //compute::copy( device_vector.begin(), device_vector.end(), host_vector.begin(), queue );
        //compute::copy( device_vector.begin(), device_vector.end(), host_vector.begin() );

    }//*/
    std::vector<float> host_vector;
    compute::vector<float> device_vector;
private:
    compute::device device;

   // compute::context context;
   // compute::command_queue queue;
};


/*template<typename Container>
class Iterator_facaded_container : public boost::iterator_facade<Iterator_facaded_container<Container>, typename Container::value_type, std::random_access_iterator_tag, ::boost::compute::detail::buffer_value<typename Container::value_type>>{
public:
    using value_type = typename Container::value_type;
    using reference = ::boost::compute::detail::buffer_value<value_type>;
    //using reference = float&;
    Iterator_facaded_container(Container &vec) : i(0), vptr(&vec){}
    Iterator_facaded_container(Container &vec, int start) : i(start), vptr(&vec){}

    void increment() { i+=1; }
    bool equal(Iterator_facaded_container const& other) const
    {
        return this->i == other.i;
    }
    reference dereference() const { return (*vptr)[i]; }
    void decrement(){i--;};
    void advance(int n ){i+=n;};
private :
    friend class boost::iterator_core_access;
    int i;
    Container *vptr;
};//*/


#define NX 3
template<typename T>
struct compute::is_device_iterator<Iterator_facaded_buff<T> > : boost::true_type {};
/*namespace boost {
    namespace compute {
        namespace detail {
            template<typename T>
            struct can_copy_with_copy_buffer<Iterator_facaded_buff<T>, Iterator_facaded_buff<T> > : boost::true_type {
            };
        }
    }
}//*/

int main()
{
    compute::command_queue queue = compute::system::default_queue();
    namespace mynm = compute;
    using boost::compute::lambda::_1;
    mynm::vector<float> bcompvec(NX, 1.f);
    mynm::vector<float> bcompvec_dest(NX, 2.f);

        // Some little tests
    /*std::vector<float> teststdvec(3, 3.f);
    compute::vector<float> testbcompvec(3, 3.f);
    ::boost::compute::detail::buffer_value<float> testbuffvalue = 3.4;
    ::boost::compute::detail::buffer_value<float> testbuffvalueread = testbcompvec[1];
    cout << testbcompvec[1] << endl;
    auto totobv = boost::addressof(testbuffvalue); //*/
    //std::iterator_traits<float *>::difference_type tata;
   /* cout << "test with stl random access vector" << endl; // works
    std::vector<float> stdvec(3, 1.f);
    std::vector<float> stdvec_dest(3, 1.f);
    stdvec[0] = 0;
    stdvec[1] = 1;
    stdvec[2] = 2;
    CIteratorFL<std::vector<float>, std::output_iterator_tag> myItbeg(stdvec, 0);
    CIteratorFL<std::vector<float>, std::output_iterator_tag> myItend(stdvec, stdvec.size());
    IteratorFL<std::vector<float>, std::output_iterator_tag> myItbeg_dest(stdvec_dest, 0);
    //IteratorFL<std::vector<float>> myItbeg_dest(stdvec_dest, 0);
   // std::copy(myItbeg, myItend, myItbeg_dest);
    cout << stdvec_dest[2] << endl;
    std::transform(myItbeg, myItend, myItbeg_dest, std::negate<float>());
    std::cout << stdvec_dest[2] << endl;
    cout << "\ttest with stl random access vector passed " << endl;//*/


    /*cout << "\ttest with boost::compute vector facaded" << endl; // works partially, not transform
    cout << "\tbcompvec[0] after creation : " << bcompvec[0] << endl;
    cout << "\tbcompvec_dest[2] after creation : " << bcompvec_dest[2] << endl;
    Iterator_facaded<compute::vector<float>> myItCompbegfac(bcompvec, 0); // works
    Iterator_facaded<compute::vector<float>> myItCompendfac(bcompvec, bcompvec.size());
   // CIterator_facaded<float> myItcCompbegfac(bcompvec, 0); // works
   // CIterator_facaded<float> myItcCompendfac(bcompvec, bcompvec.size());
    Iterator_facaded<compute::vector<float>> myItCompbegfac_dest(bcompvec_dest, 0);
    Iterator_facaded<compute::vector<float>> myItCompendfac_dest(bcompvec, bcompvec_dest.size());
    compute::copy(myItCompbegfac, myItCompendfac, myItCompbegfac_dest); // works
    queue.finish();
    cout << "\tbcompvecfac_dest[2] after copy : " << bcompvecfac_dest[2] << endl;//
    using boost::compute::lambda::_1;

   // compute::transform(myItCompbegfac, myItCompendfac, teststdvec.begin(), std::negate<float>()); // compiles
    cout << "###TRANSFORM (COMPUTEVECTOR.BEGIN...)" << endl;
    auto expr = _1+10;
    //compute::transform(myItCompbegfac, myItCompendfac, myItCompbegfac_dest, expr); // doesn't work error: no matching function for call to 'addressof' ??
    compute::transform (bcompvec.begin(), bcompvec.end(), bcompvec_dest.begin(), expr); // benchmark
    queue.finish();
    cout << "///TRANSFORM (COMPUTEVECTOR.BEGIN...)" << endl;
    cout << "\tbcompvecfac_dest[2] after transform : " << bcompvecfac_dest[2] << endl;
  //  queue.finish();
    cout << "test with boost::compute vector facaded passed" << endl;//*/




    cout << "\ttest with boost::compute vector buff facaded" << endl; // works partially, not transform
    queue.finish();
    cout << "\tbcompvecfac[0] after creation : " << bcompvec[0] << endl;
    cout << "\tbcompvecfac_dest[2] after creation : " << bcompvec_dest[2] << endl;
    Iterator_facaded_buff<float> myItCompbegfacbuff(bcompvec, 0); // works
    Iterator_facaded_buff<float> myItCompendfacbuff(bcompvec, bcompvec.size());
    // CIterator_facaded<float> myItcCompbegfac(bcompvecfac, 0); // works
    // CIterator_facaded<float> myItcCompendfac(bcompvecfac, bcompvecfac.size());
    Iterator_facaded_buff<float> myItCompbegfacbuff_dest(bcompvec_dest, 0);
    Iterator_facaded_buff<float> myItCompendfacbuf_dest(bcompvec, bcompvec_dest.size());
    //compute::copy(myItCompbegfacbuff, myItCompendfacbuff, myItCompbegfacbuff_dest); // works
    queue.finish();
    //cout << "\tbcompvecfac_dest[2] after copy : " << bcompvec_dest[2] << endl;//
    cout << "###TRANSFORM (Iterator_facaded_buff)" << endl;
    auto expr2 = _1+10;
    compute::transform(myItCompbegfacbuff, myItCompendfacbuff, myItCompbegfacbuff_dest, expr2);
    //compute::transform (bcompvec.begin(), bcompvec.end(), bcompvec_dest.begin(), expr2); // benchmark
    queue.finish();
    cout << "///TRANSFORM (Iterator_facaded_buff)" << endl;
    cout << "\tbcompvecfac_dest[2] after transform : " << bcompvec_dest[2] << endl;
    cout << "test with boost::compute vector facaded buff passed" << endl;//*/




    /*  Iterator_facaded_container<compute::vector<float>> myItCompbegfac_cont(bcompvecfac, 0); // works
    Iterator_facaded_container<compute::vector<float>> myItCompendfac_cont(bcompvecfac, bcompvecfac.size()); // works
    Iterator_facaded_container<compute::vector<float>> myItCompbegfac_dest_cont(bcompvecfac_dest, 0); // works
    Iterator_facaded_container<compute::vector<float>> myItCompendfac_dest_cont(bcompvecfac_dest, bcompvecfac_dest.size()); // works//
    //compute::transform(myItCompbegfac_dest_cont, myItCompendfac_dest_cont, myItCompbegfac_dest_cont, _1 * 3 - 4); // doesn't work error: no matching function for call to 'addressof' ??
    compute::copy(myItCompbegfac_cont, myItCompendfac_cont, myItCompbegfac_dest_cont); // works//*/


  /*  cout << "test with boost::compute vector" << endl; // doesn't work
    compute::vector<float> bcompvec(3, 1.f);
    auto mybuff = bcompvec.get_buffer();
    compute::vector<float> bcompvec_dest(3, 1.f);
    bcompvec[0] = 0;
    bcompvec[1] = 1;
    bcompvec[2] = 2;
    IteratorFL<typename compute::vector<float>> myItCompbeg(bcompvec, 0);
    IteratorFL<typename compute::vector<float>> myItCompend(bcompvec, bcompvec.size());
    IteratorFL<typename compute::vector<float>> myItCompbeg_dest(bcompvec_dest, 0);
    compute::copy(myItCompbeg, myItCompend, myItCompbeg_dest);
    cout << bcompvec_dest[2] << endl;
    bcompvec_dest[2] = 4;
    cout << bcompvec_dest[2] << endl;
 //   std::transform(myItCompbeg, myItCompend, myItCompbeg, std::negate<float>());
    cout << "test with boost::compute vector passed" << endl;//*/

 /*   cout << "test with boost::compute buffer" << endl; // works
    compute::vector<float> bcompvec2(3, 1.f);
    cout << bcompvec2[1] << endl;
    auto buff = bcompvec2.get_buffer();
    auto el = buff.get();
    auto val = compute::detail::buffer_value<float>(buff, 1 * sizeof(float));
    auto valf = float(val);
   // compute::buffer<float> toto;
    //Iterator_facaded<compute::buffer>(buff, 0);
    cout << "test with boost::compute buffer passed" << endl;//*/



    //cout << *myIt << endl;
    //cout << *(++myIt) << endl;

   /* using myfloat = typename decltype(stdvec)::value_type;
    myfloat var1;
    cout << typeid(var1).name() << endl;//

    Test_BCompute mytemp;
    mytemp.createDevVec();
    cout << mytemp.device_vector[1] << endl;
    mytemp.transform();
    cout << mytemp.device_vector[1] << endl;

    sleep(2);
    cout << mytemp.device_vector[1] << endl;
    //auto a = foo(mytemp.device_vector);

    int size = 10;
    compute::vector<float> df(size, 0);
   // Iterator<decltype(df)::value_type> it0(df, 0);
    //Iterator itEnd(df, df.size());
    /*compute::detail::buffer_value<float> toto = mytemp.device_vector[0];
    cout << mytemp.device_vector[0] << endl;
    toto = 3;
    using tototype = std::vector<float>::value_type;
    tototype var = 3.14;
    cout << mytemp.device_vector[0] << endl;//*/
   // auto begit = beg(df);


  //  float b = a;
   // cout << a << endl;
    //cout << mytemp.host_vector[1] << endl;
    return 0;
}