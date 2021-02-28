#ifndef STRING_PROCESSING_CPP_PRIORITYQUEUEEXTENDED_H
#define STRING_PROCESSING_CPP_PRIORITYQUEUEEXTENDED_H

// std::priority_queue extension to allow retrieving std::unique_ptrs
// https://stackoverflow.com/a/27000256/997151

template<typename _Tp, typename _Sequence = std::vector<_Tp>,
        typename _Compare = std::less<typename _Sequence::value_type> >
class priority_queue: std::priority_queue<_Tp, _Sequence, _Compare> {
public:
    typedef typename _Sequence::value_type value_type;
public:

#if __cplusplus < 201103L
    explicit
priority_queue(const _Compare& __x = _Compare(),
        const _Sequence& __s = _Sequence()) :
        std::priority_queue(__x, __s) {}
#else
    explicit
    priority_queue(const _Compare& __x, const _Sequence& __s) :
            std::priority_queue<_Tp, _Sequence, _Compare>(__x, __s) {}

    explicit
    priority_queue(const _Compare& __x = _Compare(), _Sequence&& __s =
    _Sequence()) :
            std::priority_queue<_Tp, _Sequence, _Compare>(__x, std::move(__s)) {}
#endif

    using std::priority_queue<_Tp, _Sequence, _Compare>::empty;
    using std::priority_queue<_Tp, _Sequence, _Compare>::size;
    using std::priority_queue<_Tp, _Sequence, _Compare>::top;
    using std::priority_queue<_Tp, _Sequence, _Compare>::push;
    using std::priority_queue<_Tp, _Sequence, _Compare>::pop;

#if __cplusplus >= 201103L

    using std::priority_queue<_Tp, _Sequence, _Compare>::emplace;
    using std::priority_queue<_Tp, _Sequence, _Compare>::swap;

/**
 *  @brief  Removes and returns the first element.
 */
    value_type pop_top() {
        __glibcxx_requires_nonempty();

        // arrange so that back contains desired
        std::pop_heap(this->c.begin(), this->c.end(), this->comp);
        value_type top = std::move(this->c.back());
        this->c.pop_back();
        return top;
    }

#endif

};


#endif //STRING_PROCESSING_CPP_PRIORITYQUEUEEXTENDED_H
