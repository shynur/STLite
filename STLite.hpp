#pragma once
#include <cstdlib>
#include <fstream>
#include <utility>
#include <iostream>
#include <filesystem>
#include <functional>
#include <type_traits>
namespace conts {
    class Cont;
    using std::swap;
    template<typename T> requires(requires(T a, T b) { std::swap(a, b); } == false && std::is_base_of<Cont, T>::value == false)
    inline void swap(const T& a, const T& b) {}

    template<typename T1, typename T2> bool operator<(const T1& a, const T2& b);
    template<typename T1, typename T2> inline bool operator<=(const T1& a, const T2& b);
    template<typename T1, typename T2> inline bool operator>=(const T1& a, const T2& b);
    template<typename T1, typename T2> inline bool operator>(const T1& a, const T2& b);
    template<typename T1, typename T2> bool operator==(const T1& a, const T2& b);
    template<typename T1, typename T2> inline bool operator!=(const T1& a, const T2& b);
    template<typename T1, typename T2> inline T1 operator+(T1 a, T2 b);

    class Cont {
      protected:
        Cont& dsize(const int det) noexcept(false);

        static int option(const char* const which);
        template<typename... cstr> static int option(const char* const which, const char* const first_option, const cstr... rest_options);
        int& npos_off(const int& num, const int target_value = npos) const;

        template<typename T> struct iterator {
            virtual iterator& operator++() = 0;
            bool operator==(const iterator& b) const { return !(*this != b); }
            virtual bool operator!=(const iterator& b) const = 0;
            virtual T& operator*() const = 0;
            virtual ~iterator() = default;
        };

      private:
        int _size;
      public:
        Cont(): _size{0} {}
        Cont(const Cont&) = delete;
        virtual ~Cont() = 0;

        static constexpr const int npos = 1 << (sizeof(int) * 8 - 1);
        [[nodiscard("information is delivered in form of const char *, but std::ios_base")]] static const char* info(const char* const which = nullptr);
        virtual bool operator<(const Cont& b) const;
        int size() const;

        static int rand(const int mod = std::rand());

        template<typename T> struct hasher {
            using argument_type = std::remove_cvref_t<T>;
            int operator()(const argument_type& to_hash) const {
                static constexpr const int default_hash_code{11'4514};
                int hash_code;
                if constexpr (requires() { int{to_hash.hash()}; })
                    hash_code = to_hash.hash();
                else if constexpr (requires() { std::hash<argument_type>{}(to_hash); })
                    hash_code = std::hash<argument_type>{}(to_hash);
                else
                    hash_code = default_hash_code;
                return hash_code & ~(Cont::npos);
            }
        };
        virtual int hash() const = 0;
    };
    template<typename T> using hasher = typename Cont::hasher<T>;

    template<typename T> class slice;
    template<typename T> class Sequential: public virtual Cont {
        int qdedup(const int lo, int hi);

        void bubble_sort(const int lo, int hi) const;
        void selection_sort(const int lo, int hi) const;
        virtual void merge_sort(int lo, const int hi) const;
        void quick_sort(const int lo, const int hi) const;
        void insertion_sort(const int lo, const int hi, const int det = 1) const;
        void Shell_sort(const int lo, const int hi) const;
        void quick_pivot(int& pivot, int hi) const;

        T& heap_kth(const int k, const int lo, const int hi) const;
        T& BFPRT_kth(const int k) const;

        int brute_grep(const slice<T>& pattern, int lo, const int hi) const;
        int KMP_grep(const slice<T>& pattern, int lo, const int hi) const;
        int BM_grep(const slice<T>& pattern, const int lo, const int hi) const;
      protected:
        virtual void heap_sort(int lo, int hi) const;
        T& quick_kth(const int& k, const int lo, const int hi) const;
      protected:
        void copy_constructor(const Sequential& b, int lo, const int hi);
        void move_constructor(Sequential&& _b) noexcept;
        void destructor();
        Sequential& dsize(const int det);
      public:
        struct iterator: Cont::iterator<T> {
            bool operator!=(const iterator& b) const { return i != b.i || psequen != b.psequen; }
            virtual bool operator!=(const Cont::iterator<T>& b) const override {
                return dynamic_cast<const iterator*>(&b) != nullptr ?
                                                                    operator!=(static_cast<const iterator&>(b))
                                                                    : throw "Sequential::iterator compared with invalid iterator";
            }
            bool operator==(const iterator& b) const { return !Cont::iterator<T>::operator!=(b); }
            virtual iterator& operator++() override {
                ++i;
                return *this;
            }
            virtual T& operator*() const override { return (*psequen)[i]; }
            iterator(const iterator& b) = default;
            virtual ~iterator() override = default;
          private:
            const Sequential* psequen;
            int i;
            friend class Sequential;
            iterator(const Sequential* const psequen, const int i): Cont::iterator<T>{}, psequen{psequen}, i{i} {}
        };
        friend void swap(Sequential& a, Sequential& b) {
            if (a.size() <= b.size() == false)
                return swap(b, a);
            int hi{a.size()};
            for (int lo{0}; lo < hi; ++lo)
                swap(a[lo], b[lo]);
            while (hi < b.size())
                a.push(b.pick(hi));
        }

        virtual bool operator<(const Cont& b) const final override;
        bool operator<(const Sequential& b) const;

        virtual T& operator[](const int i) const noexcept(false) = 0;
        virtual Sequential& push(T e) = 0;
        virtual T pop() noexcept(noexcept(operator[](0))) = 0;

        virtual Sequential& insert(int i, T e);
        virtual T pick(const int i);
        Sequential& pick(const int lo, int hi);

        template<typename F> F& traverse(const F& cref_f, int lo = 0, const int hi = npos) const noexcept(noexcept(F::operator()));
        int count(const T& e, const int lo = 0, const int hi = npos) const;
        T& major(int lo = 0, const int hi = npos) const;

        Sequential& shuffle(const int lo = 0, const int hi = npos) const;
        Sequential& reverse(int lo = 0, int hi = npos) const;
        bool sorted(int lo = 0, const int hi = npos) const;
        int find(const T& e, int lo = 0, const int hi = npos) const;
        int qfind(const T& e) const;
        int dedup(const int lo = 0, int hi = npos);

        Sequential& sort(const int lo = 0, const int hi = npos) const;
        Sequential& sort(const char* const which, const int lo = 0, const int hi = npos) const;

        T& top_k(const int k = npos, const char* which = nullptr) const;
        T& min() const;
        T& max() const;

        int grep(const slice<T>& pattern, const char* which) const;
        int grep(const slice<T>& pattern, const int lo = 0, int hi = npos, const char* which = nullptr) const;

        iterator begin() const;
        iterator end() const;
        virtual int hash() const override;
    };

    template<typename T> class Vector: public virtual Sequential<T> {
        using Sequential = Sequential<T>;
        virtual void heap_sort(const int lo, const int hi) const override;
      protected:
        using Sequential::npos_off;
        Vector& dsize(const int det);
      private:
        struct Block final {
            Block(): bytes{} {}
            operator T*() const { return reinterpret_cast<T*>(const_cast<Block*>(this)); }
            static T*& clr(T* const& p, const int num = 1) {
                new (p) char[num * sizeof(T)]{};
                return const_cast<T*&>(p);
            }
            static T*& del(T*& p) {
                delete[] reinterpret_cast<Block*&>(p);
                return p = nullptr;
            }
          private:
            char bytes[sizeof(T)];
        };

        T* elem;
        int capacity;

        Vector& destruct(const int lo, int hi = Sequential::npos) const;
        Vector& scale();
      public:
        using Sequential::npos;
        using Sequential::size;
        virtual Vector& insert(const int i, T e) override;
        using Sequential::pick;
        Vector& pick(const int lo, const int hi);
        Vector& shuffle(const int lo = 0, const int hi = npos) const;
        Vector& reverse(const int lo = 0, const int hi = npos) const;
        Vector& sort(const int lo = 0, const int hi = npos) const;
        Vector& sort(const char* const which, const int lo = 0, const int hi = npos) const;
      public:
        Vector(const char* = nullptr): Sequential{}, elem{*(new Block[1])}, capacity{1} {}
        Vector(const Sequential& b, const int lo = 0, const int hi = npos): Vector{} { Sequential::copy_constructor(b, lo, hi); }
        Vector(const Vector& b, const int lo = 0, const int hi = npos): Vector{static_cast<const Sequential&>(b), lo, hi} {}
        Vector(Sequential&& _b) noexcept: Vector{} { Sequential::move_constructor(_b); }
        Vector(Vector&& _b) noexcept: Vector{} {
            swap(dsize(_b.size()).elem, _b.elem), capacity = _b.dsize(-_b.size()).capacity, _b.capacity = 0, Block::del(_b.elem);
        }
        friend void swap(Vector& a, Vector& b) {
            int a_size{a.size()};
            swap(a.dsize(b.size() - a_size).elem, b.elem), swap(a.capacity, b.dsize(a_size - b.size()).capacity);
        }
        Vector& operator=(Vector b);
        virtual ~Vector() override;

        template<typename iterable_T> Vector(const iterable_T& src): Vector{} {
            for (const auto& e : src)
                push(e);
        }
        template<typename iterable_T> Vector(iterable_T&& src): Vector{} {
            for (auto& e : src)
                push(static_cast<std::remove_cvref_t<decltype(e)>&&>(e));
        }

        virtual T& operator[](const int i) const noexcept(false&& noexcept(Sequential::operator[](0))) override;
        virtual Vector& push(T e) override;
        virtual T pop() noexcept(noexcept(operator[](0))) override;

        using Sequential::operator<;
        bool operator<(const Vector& b) const;

        Vector& operator+=(Vector b);
        operator const T*() const;
        Vector(const unsigned char* s): Vector{reinterpret_cast<const char*>(s)} {}
    };
    using String = Vector<char>;
    using uString = Vector<unsigned char>;
    using BitMap = Vector<bool>;

    namespace auto_deref {
        template<typename T> struct _AutoDeref {
            using base_type = _AutoDeref;
            _AutoDeref(const T& e): p{&e} {}
            friend void swap(_AutoDeref& a, _AutoDeref& b) { swap(a.p, b.p); }
            operator const T&() const { return *p; }
            bool operator<(const _AutoDeref& b) const { return *p < b; }
            bool operator==(const _AutoDeref& b) const { return *p == static_cast<T>(b); }
          private:
            const T* p;
        };
        template<typename T> struct _AutoDeref<_AutoDeref<T>>: _AutoDeref<T> {};
        template<typename T> using AutoDeref = _AutoDeref<std::remove_cvref_t<T>>::base_type;
    } // namespace auto_deref
    template<typename T> class slice final: public Vector<auto_deref::AutoDeref<T>> {
        friend class Sequential<T>;
        using Vector_p = Vector<auto_deref::AutoDeref<T>>;
        using Vector_p::quick_kth;
      public:
        slice(): Vector_p{} {}
        slice(const slice& b, const int lo = 0, const int hi = Vector_p::npos): Vector_p{b, lo, hi} {}
        slice(slice&& _b): Vector_p{static_cast<Vector_p&&>(_b)} {}
        template<typename iterable_T> slice(const iterable_T& c, int lo = 0, int hi = ~Vector_p::npos): slice{} {
            for (const auto& e : c)
                if (0 < lo)
                    --lo, --hi;
                else if (0 < hi--)
                    Vector_p::push(e);
                else
                    break;
        }
        friend void swap(slice& a, slice& b) { swap(static_cast<Vector_p&>(a), static_cast<Vector_p&>(b)); }
        slice& operator=(slice b);
        virtual ~slice() override = default;
        slice& operator+=(slice b);
        using Vector_p::operator<;
        bool operator<(const slice& b) const;
        virtual auto_deref::AutoDeref<T>& operator[](const int i) const noexcept(noexcept(Vector_p::operator[])) override;
    };

    template<typename T> class List: public virtual Sequential<T> {
        using Sequential = Sequential<T>;
      protected:
        using Sequential::npos_off;
        List& dsize(const int det);
      private:
        struct _Node {
            _Node(_Node* const pleft, _Node* const pright): pleft{pleft}, pright{pright} { pleft == nullptr ? nullptr : (pleft->pright = this), pright == nullptr ? nullptr : (pright->pleft = this); }
            _Node(_Node&& _b): _Node{_b.pleft, _b.pright} { _b.pleft = _b.pright = nullptr; }
            virtual ~_Node() { pleft == nullptr ? nullptr : (pleft->pright = pright), pright == nullptr ? nullptr : (pright->pleft = pleft), pleft = pright = nullptr; }

            T& data();
            auto& insert_at(_Node& inserted_at);
            _Node *pleft, *pright;
        };
        struct Node: _Node {
            Node(_Node& left, T _data, _Node& right): _Node{&left, &right}, _data{static_cast<T&&>(_data)} {}
            Node(Node&& _b): _Node{static_cast<_Node&&>(_b)}, _data{static_cast<T&&>(_b._data)} {}
            virtual ~Node() override = default;

            T _data;
        };
        _Node head, tail;
        mutable struct {
            int rank;
            _Node* p;
            auto& go(int det) {
                if (rank += det; det >= 0)
                    while (0 != det--)
                        p = p->pright;
                else
                    while (det++ != 0)
                        p = p->pleft;
                return *this;
            }
        } vernier;

      public:
        using Sequential::npos;
        using Sequential::size;
      public:
        List(): Sequential{}, head{nullptr, &tail}, tail{&head, nullptr}, vernier{0, &tail} {}
        List(const Sequential& b, const int lo = 0, const int hi = npos): List{} { Sequential::copy_constructor(b, lo, hi); }
        List(const List& b, const int lo = 0, const int hi = npos): List{static_cast<const Sequential&>(b), lo, hi} {}
        List(Sequential&& _b) noexcept: List{} { Sequential::move_constructor(static_cast<Sequential&&>(_b)); }
        List(List&& _b) noexcept: List{} { _b.size() == 0 ? nullptr : (dsize(_b.size()).head.pright = &(_b.at(0)), tail.pleft = &(_b.at(_b.size() - 1)), _b.at(0).pleft = &head, _b.at(size() - 1).pright = &tail, _b.dsize(-_b.size()).head.pright = &(_b.tail), _b.tail.pleft = &(_b.head)); }
        friend void swap(List& a, List& b) {
            if (a.size() == 0 && b.size() == 0)
                ;
            else if (a.size() == 0)
                return swap(b, a);
            else if (_Node & a_first{a.at(0)}, &a_last{a.at(a.size() - 1)}, &b_first{b.at(0)}, &b_last{b.at(b.size() - 1)}; b.size() != 0)
                new (&(a.head)) _Node{nullptr, &b_first}, new (&(a.tail)) _Node{&b_last, nullptr}, new (&(b.head)) _Node{nullptr, &a_first}, new (&(b.tail)) _Node{&a_last, nullptr};
            else
                new (&(b.head)) _Node{nullptr, &a_first}, new (&(b.tail)) _Node{&a_last, nullptr}, a.head.pright = &(a.tail), a.tail.pleft = &(a.head);
            const int det{b.size() - a.size()};
            swap(a.vernier, b.vernier), a.dsize(det), b.dsize(-det);
        }
        List& operator=(List b);
        virtual ~List() override;

        template<typename iterable_T> List(const iterable_T& src): List{} {
            for (const auto& e : src)
                push(e);
        }
        template<typename iterable_T> List(iterable_T&& src): List{} {
            for (auto& e : src)
                push(static_cast<std::remove_cvref_t<decltype(e)>&&>(e));
        }

        virtual T& operator[](const int i) const noexcept(false&& noexcept(Sequential::operator[](0))) override;
        virtual List& push(T e) override;
        virtual T pop() noexcept(noexcept(operator[](0))) override;
        T deque() noexcept(noexcept(operator[](0)));

        using Sequential::operator<;
        bool operator<(const List& b) const;

        _Node& at(const int i) const;

        virtual List& insert(const int i, T e) override;
        List& insert(_Node& node_i, T e, const int i = npos);

        using Sequential::pick;
        virtual T pick(const int i) override;
        T pick(_Node& node_i, const int i = npos);
      private:
        virtual void merge_sort(const int lo, const int hi) const override;
        void merge_sort(int lo, int mi, const int cmi, const int chi, _Node* plo_left, _Node* plo, _Node* pmi, _Node* const cphi) const;
    };

    template<typename vertex_data_T, typename edge_data_T> class Graph;
    namespace component_of_graph {

        template<typename vertex_data_T, typename Edge_T> struct Vertex;
        template<typename vertex_data_T, typename T> struct Edge final {
            using value_type = T;
            int dist;
            enum { UNDETERMINED,
                   TREE,
                   CROSS,
                   FORWARD,
                   BACKWARD } type
                = UNDETERMINED;
            Edge(const int distance, const int destination): dist{distance}, _dest{destination} {}
            friend void swap(Edge& a, Edge& b) { swap(a.dist, b.dist), swap(a.type, b.type), swap_data(a, b), swap(const_cast<int&>(a._dest), const_cast<int&>(b._dest)); }
            bool has_data() const;
            const struct {
                const auto& operator=(T data) const {
                    if (auto& this_data{this->data}; this_data.size() == 0)
                        this_data.push(static_cast<T&&>(data));
                    else
                        swap(this_data[0], data);
                    return *this;
                }
                operator T&() const { return data[0]; }
                operator T&&() const { return static_cast<T&&>(data[0]); }
              private:
                mutable Vector<T> data;
                bool has_data() const { return data.size() != 0; }
                int hash() const { return data.hash(); }
                friend struct Edge<vertex_data_T, T>;
            } data;
            int dest() const;
            int hash() const;
          private:
            const int _dest;
            void reset();
            static void swap_data(Edge& a, Edge& b);
            friend struct Vertex<vertex_data_T, Edge>;
        };
        template<typename T, typename Edge_T> struct Vertex final: private Vector<Edge_T> {
            T data;
            int parent, priority;
            mutable int tag1 = 0, tag2 = 0;
            enum { UNDISCOVERED,
                   DISCOVERED,
                   VISITED } state
                = UNDISCOVERED;
            Vertex(T data, const int _rank, const Graph<T, typename Edge_T::value_type>& graph): Vector<Edge_T>{}, data{static_cast<T&&>(data)}, parent{Vector<Edge_T>::npos}, priority{~Vector<Edge_T>::npos}, in_degree{0}, _rank{_rank}, pgraph{&graph} {}
            Vertex(const Vertex& b): Vector<Edge_T>{b}, data{b.data}, parent{b.parent}, priority{b.priority}, tag1{b.tag1}, tag2{b.tag2}, state{b.state}, in_degree{b.in_degree}, _rank{b.rank()}, pgraph{b.pgraph} {}
            Vertex(Vertex&& _b) noexcept: Vector<Edge_T>{static_cast<Vector<Edge_T>&&>(_b)}, data{static_cast<T&&>(_b.data)}, parent{_b.parent}, priority{_b.priority}, tag1{_b.tag1}, tag2{_b.tag2}, state{_b.state}, in_degree{_b.in_degree}, _rank{_b.rank()}, pgraph{_b.pgraph} {}
            friend void swap(Vertex& a, Vertex& b) { swap(static_cast<Vector<Edge_T>&>(a), static_cast<Vector<Edge_T>&>(b)), swap(a.data, b.data), swap(a.parent, b.parent), swap(a.priority, b.priority), swap(a.tag1, b.tag1), swap(a.tag2, b.tag2), swap(a.state, b.state), swap(a.in_degree, b.in_degree), swap(a._rank, b._rank), swap(const_cast<const Graph<T, typename Edge_T::value_type>*&>(a.pgraph), const_cast<const Graph<T, typename Edge_T::value_type>*&>(b.pgraph)); }

            bool has_edge_to(const int dest) const;
            Edge_T& operator()(const int dest) const;
            Edge_T cut(const int dest);

            int ideg() const;
            int odeg() const;
            Vertex& operator>>(const Vertex& dest);
            [[nodiscard]] auto operator>>(const int dist);
            int rank() const;
            bool cyclic() const;

            using Vector<Edge_T>::begin;
            using Vector<Edge_T>::end;
            virtual int hash() const override;
          private:
            mutable int in_degree, _rank;
            const Graph<T, typename Edge_T::value_type>* const pgraph;
            void reset();
            virtual Vertex& push(Edge_T edge) override;
            friend class Graph<T, typename Edge_T::value_type>;
        };

    } // namespace component_of_graph
    template<typename vertex_data_T, typename edge_data_T> class Graph: private Vector<component_of_graph::Vertex<vertex_data_T, component_of_graph::Edge<vertex_data_T, edge_data_T>>>, public virtual Cont {
        using Edge = component_of_graph::Edge<vertex_data_T, edge_data_T>;
        using Vertex = component_of_graph::Vertex<vertex_data_T, Edge>;
        using Vector_v = Vector<Vertex>;

        using Vector_v::push;
        void update() const;

        using Cont::operator<;
        using Vector_v::operator<;
      public:
        int default_distance = 8;
        Graph& reset(const bool clear_priority = false) const;
        Graph& undirect() const;

        using Vector_v::size;
        using Vector_v::operator[];
        using Vector_v::begin;
        using Vector_v::end;
        bool operator<(const Graph& b) const;

        Graph& push(vertex_data_T v_data);
        virtual Vertex pick(const int i) override;

        int cnt_edges() const;
        int find(const vertex_data_T& vertex_data, int lo = 0, const int hi = Vector_v::npos) const;
      public:
        Graph() = default;
        Graph(const Graph& b): Vector_v{b}, default_distance{b.default_distance}, clk{b.clk} { update(); }
        Graph(Graph&& _b) noexcept: Vector_v{static_cast<Vector_v&&>(_b)}, default_distance{_b.default_distance}, clk{_b.clk} { update(); }
        friend void swap(Graph& a, Graph& b) { swap(a.default_distance, b.default_distance), swap(static_cast<Vector_v&>(a), static_cast<Vector_v&>(b)), swap(a.clk, b.clk), a.update(), b.update(); }
        Graph& operator=(Graph b) {
            swap(*this, b);
            return *this;
        }
        virtual ~Graph() override = default;

        template<typename F> F& BFS(const F& cref_f, const int u = Vector_v::npos) const;
        template<typename F> F& DFS(const F& cref_f, const int u = Vector_v::npos) const;
        template<typename Priority_updater_T> Priority_updater_T& PFS(const Priority_updater_T& cref_pu, const int u = Vector_v::npos) const;

        Vector<auto> Prim(const int u = 0) const;
        Vector<auto> Dijkstra(const int u = 0) const;
        Vector<auto> topo_sort(const int v = Vector_v::npos) const;
        Vector<Vector<auto>> bcc() const;

        static constexpr const auto VISITED = Vertex::VISITED;
        static constexpr const auto DISCOVERED = Vertex::DISCOVERED;
        static constexpr const auto UNDISCOVERED = Vertex::UNDISCOVERED;
        static constexpr const auto TREE = Edge::TREE;
        static constexpr const auto CROSS = Edge::CROSS;
        static constexpr const auto FORWARD = Edge::FORWARD;
        static constexpr const auto BACKWARD = Edge::BACKWARD;
        static constexpr const auto UNDETERMINED = Edge::UNDETERMINED;

        auto from(const int origin) const;
        virtual int hash() const override;
      private:
        mutable int clk = 0;
        template<typename F, typename XFS> F& search(const F& cref_f, const XFS x_first_search, int u) const;

        template<typename F> void breadth_first_search(F& f, const int u) const;
        template<typename F> void depth_first_search(F& f, const int u) const;
        template<typename Priority_updater_T> void priority_first_search(Priority_updater_T& pu, int u) const;
    };

    template<typename K, typename V> struct Dictionary: public virtual Cont {
        struct Entry final {
            const K key;
            V val;
            Entry(const K& k, V v): key{k}, val{static_cast<V&&>(v)} {}
            bool operator==(const K& b_key) const { return key == b_key; }
            friend bool operator==(const K& a_key, const Entry& b) { return b == a_key; }
        };

        virtual Dictionary& insert(const K& key, V val) = 0;
        virtual bool operator&(const K& key) const = 0;
        virtual V& operator[](const K& key) const noexcept(false) = 0;
        virtual Entry pick(const K& key) noexcept(false) = 0;
    };
    template<typename K, typename V> class Hash: public Dictionary<K, V> {
      protected:
        using Dictionary = Dictionary<K, V>;
        using typename Dictionary::Entry;

        struct Bucket final {
            bool empty;
            Entry* ptr_entry;

            Bucket(): empty{true}, ptr_entry{nullptr} {}
            Bucket(Bucket&& _b): empty{_b.empty}, ptr_entry{_b.ptr_entry} { _b.empty = true, _b.ptr_entry = nullptr; }
            friend void swap(Bucket& a, Bucket& b) { swap(a.empty, b.empty), swap(a.ptr_entry, b.ptr_entry); }
            ~Bucket();

            bool brand_new() const;
            Bucket& push(const K& key, V val);
            Entry pop();
            int hash() const;
        };
        Hash& dsize(const int det);
        struct iterator {
            bool operator==(const iterator& b) const { return i == b.i; }
            bool operator!=(const iterator& b) const { return !(*this == b); }
            Entry& operator*() const { return *(cref_buckets[i].ptr_entry); }
            iterator& operator++() {
                i < cref_buckets.size() ? ++i : 0;
                while (i < cref_buckets.size() && cref_buckets[i].empty)
                    ++i;
                return *this;
            }
          private:
            const Vector<Bucket>& cref_buckets;
            int i;
            iterator(const Vector<Bucket>& buckets, const int i): cref_buckets{buckets}, i{i} {}
            friend class Hash;
        };

      private:
        using Cont::operator<;

        Vector<Bucket> buckets;
        mutable int number_of_used_buckets;
        mutable bool is_scaling = false;

        int MAD(const K& key) const;
        Hash& scale() const;
        int locate(const int origin, const int sq_base) const;
        int find(const K& key) const noexcept(false);
      public:
        Hash(): Dictionary{}, buckets{}, number_of_used_buckets{0} { buckets.push({}).push({}); }
        Hash(const Hash& b): Hash{} {
            for (const auto& [key, val] : b)
                insert(key, val);
        }
        Hash(Hash&& _b) noexcept: Dictionary{}, buckets{static_cast<Vector<Bucket>&&>(_b.buckets)}, number_of_used_buckets{_b.number_of_used_buckets} { _b.dsize(-dsize(_b.size()).size()).number_of_used_buckets = 0; }
        friend void swap(Hash& a, Hash& b) { swap(a.dsize(-b.dsize(-a.dsize(b.size()).size()).dsize(-b.size() * 2).size()).buckets, b.buckets), swap(a.number_of_used_buckets, b.number_of_used_buckets); }
        Hash& operator=(Hash b) {
            swap(*this, b);
            return *this;
        }
        virtual ~Hash() override = default;

        virtual Hash& insert(const K& key, V val) override;
        virtual bool operator&(const K& key) const override;
        virtual V& operator[](const K& key) const noexcept(false) override;
        virtual Entry pick(const K& key) noexcept(false) override;

        using Cont::size;
        bool operator<(const Hash& b) const;
        iterator begin() const;
        iterator end() const;
        virtual int hash() const override;

        auto load_factor() const;
    };

    template<typename T> struct PriorityQueue: public virtual Cont {
        virtual PriorityQueue& enque(T e) = 0;
        virtual T deque() noexcept(false) = 0;
        virtual const T& front() const noexcept(false) = 0;
    };
    template<typename T> class CHeap: private Vector<T>, public virtual PriorityQueue<T> {
        friend class Vector<T>;
        CHeap(const bool _, Vector<T>&& _b);
        static CHeap Floyd(Vector<T>& to_heapify);

        static int parent_of(int child);
        static int left_child_of(int parent);
        static int right_child_of(int parent);

        CHeap& percolate_up() const;
        void percolate_down(int parent) const;
      public:
        CHeap() = default;
        CHeap(const CHeap& b): Vector<T>{static_cast<const Vector<T>&>(b)} {}
        CHeap(CHeap&& _b) noexcept: Vector<T>{static_cast<Vector<T>&&>(_b)} {}
        friend void swap(CHeap& a, CHeap& b) { swap(static_cast<Vector<T>&>(a), static_cast<Vector<T>&>(b)); }
        CHeap& operator=(CHeap b) {
            swap(*this, b);
            return *this;
        }
        virtual ~CHeap() override = default;

        CHeap(Vector<T> to_heapify): CHeap{Floyd(to_heapify)} {}

        using Vector<T>::size;
        using Vector<T>::count;
        using Vector<T>::major;
        using Vector<T>::find;
        using Vector<T>::max;
        using Vector<T>::top_k;

        using Vector<T>::operator<;
        bool operator<(const CHeap& b) const;
        const T& at(const int i) const noexcept(false);
        auto begin() const;
        auto end() const -> decltype(begin());
        CHeap& operator+=(CHeap b);
        using Vector<T>::hash;

        virtual CHeap& enque(T e) override;
        virtual T deque() noexcept(false) override;
        virtual const T& front() const noexcept(false) override;
    };
} // namespace conts
namespace conts {
    /* impl for Heap */

    template<typename T> CHeap<T> CHeap<T>::Floyd(Vector<T>& to_heapify) {
        CHeap heapified{false, static_cast<Vector<T>&&>(to_heapify)};
        for (int hi{heapified.size() - 1}; hi >= 0; --hi)
            heapified.percolate_down(hi);
        return heapified;
    }
    template<typename T> CHeap<T>& CHeap<T>::percolate_up() const {
        for (int child{size() - 1}, parent{parent_of(child)}; parent >= 0; swap((*this)[child], (*this)[parent]), parent = parent_of(child = parent))
            if ((*this)[child] >= (*this)[parent])
                break;
        return const_cast<CHeap&>(*this);
    }
    template<typename T> void CHeap<T>::percolate_down(int parent) const {
        const int siz{size()};
        const auto right_is_better{[&siz](const int& parent) -> int {
            int rchild{right_child_of(parent)};
            return rchild != siz ? rchild : --rchild;
        }};
        for (int lchild{left_child_of(parent)}, rchild{right_is_better(parent)}; lchild < siz; lchild = left_child_of(parent), rchild = right_is_better(parent))
            if (T & parent_elem{(*this)[parent]}; parent_elem <= (*this)[lchild] && parent_elem <= (*this)[rchild])
                break;
            else if (T & lchild_elem{(*this)[lchild]}, &rchild_elem{(*this)[rchild]}; lchild_elem <= rchild_elem)
                swap(parent_elem, lchild_elem), parent = lchild;
            else
                swap(parent_elem, rchild_elem), parent = rchild;
    }

    template<typename T> CHeap<T>& CHeap<T>::enque(T e) { return static_cast<CHeap&>(Vector<T>::push(static_cast<T&&>(e))).percolate_up(); }
    template<typename T> const T& CHeap<T>::front() const noexcept(false) { return at(0); }
    template<typename T> T CHeap<T>::deque() noexcept(false) {
        T to_deque{(swap((*this)[0], (*this)[size() - 1]), Vector<T>::pop())};
        percolate_down(0);
        return to_deque;
    }

    template<typename T> int CHeap<T>::parent_of(int child) { return --child / 2; }
    template<typename T> int CHeap<T>::left_child_of(int parent) { return (parent *= 2) += 1; }
    template<typename T> int CHeap<T>::right_child_of(int parent) { return (parent *= 2) += 2; }
    template<typename T> const T& CHeap<T>::at(const int i) const noexcept(false) { return (*this)[i]; }
    template<typename T> CHeap<T>& CHeap<T>::operator+=(CHeap b) {
        for (size() < b.size() ? swap(*this, b) : [] {}(); b.size() != 0;)
            enque(b.deque());
        return *this;
    }

    template<typename T> bool CHeap<T>::operator<(const CHeap& b) const { return Vector<T>::operator<(static_cast<const Vector<T>&>(b)); }
    template<typename T> CHeap<T>::CHeap(const bool _, Vector<T>&& _b): Vector<T>{static_cast<Vector<T>&&>(_b)} {}
    template<typename T> auto CHeap<T>::begin() const {
        struct iterator {
            const CHeap& heap;
            int i;
            bool operator==(const iterator& b) const { return i == b.i; }
            bool operator!=(const iterator& b) const { return !(*this == b); }
            iterator& operator++() {
                ++i;
                return *this;
            }
            const T& operator*() const { return heap.at(i); }
        } beg{*this, 0};
        return beg;
    }
    template<typename T> auto CHeap<T>::end() const -> decltype(begin()) {
        auto beg{begin()};
        while (beg.i != size())
            ++(beg.i);
        return beg;
    }

} // namespace conts
namespace conts {
    /* impl for Hash */

    template<typename K, typename V> Hash<K, V>& Hash<K, V>::insert(const K& key, V val) {
        try {
            swap((*this)[key], val);
        } catch (...) {
            int location{};
            for (int hit{(is_scaling ? *this : scale()).MAD(key)}, sq_base{0}; buckets[location = locate(hit, sq_base)].empty == false; ++sq_base)
                ;
            buckets[location].push(key, static_cast<V&&>(val)), ++dsize(1).number_of_used_buckets;
        }
        return *this;
    }
    template<typename K, typename V> bool Hash<K, V>::operator&(const K& key) const {
        try {
            (*this)[key];
        } catch (...) {
            return false;
        }
        return true;
    }
    template<typename K, typename V> V& Hash<K, V>::operator[](const K& key) const noexcept(false) { return buckets[find(key)].ptr_entry->val; }
    template<typename K, typename V> Dictionary<K, V>::Entry Hash<K, V>::pick(const K& key) noexcept(false) {
        auto [k, v]{buckets[find(key)].pop()};
        dsize(-1).scale();
        return {k, static_cast<V&&>(v)};
    }

    template<typename K, typename V> int Hash<K, V>::MAD(const K& key) const {
        static constexpr const int k{1}, b{0};
        return ((k * (hasher<K>{}(key)) + b) & ~Dictionary::npos) % buckets.size();
    }
    template<typename K, typename V> Hash<K, V>& Hash<K, V>::dsize(const int det) { return dynamic_cast<Hash&>(Dictionary::dsize(det)); }
    template<typename K, typename V> Hash<K, V>& Hash<K, V>::scale() const {
        static constexpr const int prime_numbers[]{2, 2, 5, 11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 1'2853, 2'5717, 5'1437, 10'2877, 20'5759, 41'1527, 82'3117, 164'6237, 329'2489, 658'4983, 1316'9977, 2633'9969, 5267'9969, 1'0535'9939, 2'1071'9881, 4'2143'9783, 8'4287'9579, 16'8575'9167};
        static const Vector<int> primes{prime_numbers};
        int old_size_buckets{buckets.size()}, new_size_buckets{};
        if (is_scaling = true; 2 * (number_of_used_buckets + 1) > old_size_buckets)
            new_size_buckets = prime_numbers[primes.qfind(old_size_buckets)];
        else if (const int last_size_buckets{primes[primes.qfind(old_size_buckets) - 2]}; last_size_buckets / 4 <= size())
            return is_scaling = false, const_cast<Hash&>(*this);
        else
            new_size_buckets = last_size_buckets;
        Vector<Bucket> new_buckets{}, &old_buckets{new_buckets};
        for (int i{0}; i < new_size_buckets; ++i)
            new_buckets.push({});
        swap(const_cast<Hash*>(this)->dsize(-size()).buckets, new_buckets), number_of_used_buckets = 0;
        for (auto& bucket : old_buckets)
            if (bucket.empty == false) {
                auto [key, val]{bucket.pop()};
                const_cast<Hash*>(this)->insert(key, static_cast<V&&>(val));
            }
        return is_scaling = false, const_cast<Hash&>(*this);
    }

    template<typename K, typename V> bool Hash<K, V>::operator<(const Hash& b) const { return &b < this; }
    template<typename K, typename V> auto Hash<K, V>::load_factor() const { return 1.0 * size() / buckets.size(); }
    template<typename K, typename V> typename Hash<K, V>::iterator Hash<K, V>::begin() const { return ++(iterator{buckets, -1}); }
    template<typename K, typename V> typename Hash<K, V>::iterator Hash<K, V>::end() const { return {buckets, buckets.size()}; }
    template<typename K, typename V> int Hash<K, V>::hash() const { return Cont::hash() ^ buckets.hash(); }

    template<typename K, typename V> int Hash<K, V>::locate(const int origin, const int sq_base) const { return (origin + sq_base * sq_base) % buckets.size(); }
    template<typename K, typename V> int Hash<K, V>::find(const K& key) const noexcept(false) {
        for (int hit{MAD(key)}, sq_base{0}, location{locate(hit, sq_base)}; buckets[location].brand_new() == false; location = locate(hit, ++sq_base))
            if (const Bucket & bucket{buckets[location]}; bucket.empty == false && *(bucket.ptr_entry) == key)
                return location;
        throw "entry with the key does not exist";
    }

    template<typename K, typename V> Hash<K, V>::Bucket::~Bucket() {
        if (!empty)
            delete ptr_entry, ptr_entry = nullptr;
    }
    template<typename K, typename V> bool Hash<K, V>::Bucket::brand_new() const { return ptr_entry == nullptr; }
    template<typename K, typename V> Hash<K, V>::Bucket& Hash<K, V>::Bucket::push(const K& key, V val) {
        if (brand_new())
            ptr_entry = new Entry{key, static_cast<V&&>(val)};
        else if (empty)
            ptr_entry->~auto(), new (ptr_entry) Entry{key, static_cast<V&&>(val)};
        else
            pop(), push(key, static_cast<V&&>(val));
        empty = false;
        return *this;
    }
    template<typename K, typename V> Dictionary<K, V>::Entry Hash<K, V>::Bucket::pop() {
        empty = true;
        return {ptr_entry->key, static_cast<V&&>(ptr_entry->val)};
    }
    template<typename K, typename V> int Hash<K, V>::Bucket::hash() const { return hasher<bool>{}(empty) ^ hasher<Entry*>{}(ptr_entry); }

} // namespace conts
namespace conts {
    /* impl for Graph */

    template<typename vertex_data_T, typename T> bool component_of_graph::Edge<vertex_data_T, T>::has_data() const { return data.has_data(); }
    template<typename vertex_data_T, typename T> void component_of_graph::Edge<vertex_data_T, T>::reset() { type = UNDETERMINED; }
    template<typename vertex_data_T, typename T> int component_of_graph::Edge<vertex_data_T, T>::dest() const { return _dest; }
    template<typename vertex_data_T, typename T> void component_of_graph::Edge<vertex_data_T, T>::swap_data(Edge& a, Edge& b) { swap(a.data.data, b.data.data); }
    template<typename vertex_data_T, typename T> int component_of_graph::Edge<vertex_data_T, T>::hash() const { return hasher<int>{}(dist) ^ data.hash(); }

    template<typename T, typename Edge_T> bool component_of_graph::Vertex<T, Edge_T>::has_edge_to(const int dest) const {
        for (const auto& edge : *this)
            if (edge.dest() == dest)
                return true;
        return false;
    }
    template<typename T, typename Edge_T> Edge_T& component_of_graph::Vertex<T, Edge_T>::operator()(const int dest) const {
        for (auto& edge : *this)
            if (edge.dest() == dest)
                return edge;
        throw "the cut does not exist";
    }
    template<typename T, typename Edge_T> Edge_T component_of_graph::Vertex<T, Edge_T>::cut(const int dest) {
        for (int i{0}; i < odeg(); ++i)
            if ((*this)[i].dest() == dest) {
                --(*pgraph)[dest].in_degree;
                return Vector<Edge_T>::pick(i);
            }
        static constexpr const char* const err_info{"edge to cut does not exist"};
        std::cerr << err_info << '\n', throw err_info;
    }
    template<typename T, typename Edge_T> int component_of_graph::Vertex<T, Edge_T>::ideg() const { return in_degree; }
    template<typename T, typename Edge_T> int component_of_graph::Vertex<T, Edge_T>::odeg() const { return Vector<Edge_T>::size(); }
    template<typename T, typename Edge_T> component_of_graph::Vertex<T, Edge_T>& component_of_graph::Vertex<T, Edge_T>::operator>>(const Vertex& dest) { return (*this >> pgraph->default_distance >> dest); }
    template<typename T, typename Edge_T> [[nodiscard]] auto component_of_graph::Vertex<T, Edge_T>::operator>>(const int dist) {
        struct edge_maker final {
            [[nodiscard]] edge_maker& operator>>(const int dist) {
                this->dist += dist;
                return *this;
            }
            Vertex& operator>>(const Vertex& dest) {
                origin.push({dist, dest.rank()}), ++dest.in_degree;
                return const_cast<Vertex&>(dest);
            }
          private:
            Vertex& origin;
            int dist;
            friend struct Vertex<T, Edge_T>;
            edge_maker(Vertex& origin, const int dist): origin{origin}, dist{dist} {}
        };
        return edge_maker{*this, dist};
    }
    template<typename T, typename Edge_T> int component_of_graph::Vertex<T, Edge_T>::rank() const { return _rank; }
    template<typename T, typename Edge_T> void component_of_graph::Vertex<T, Edge_T>::reset() {
        for (auto& edge : *this)
            edge.reset();
        parent = Vector<Edge_T>::npos, state = UNDISCOVERED;
    }
    template<typename T, typename Edge_T> component_of_graph::Vertex<T, Edge_T>& component_of_graph::Vertex<T, Edge_T>::push(Edge_T edge) {
        if (has_edge_to(edge.dest()))
            return swap((*this)(edge.dest()), edge), *this;
        else
            return static_cast<Vertex&>(Vector<Edge_T>::push(static_cast<Edge_T&&>(edge)));
    }
    template<typename T, typename Edge_T> bool component_of_graph::Vertex<T, Edge_T>::cyclic() const {
        Vector<int> visited{};
        for (List<int> to_visit{List<int>{}.push(rank())}; to_visit.size() != 0; to_visit.deque())
            for (const auto& edge : (*pgraph)[to_visit[0]])
                if (const int edge_dest{edge.dest()}; edge_dest == rank())
                    return true;
                else if (visited.find(edge_dest) == visited.npos)
                    to_visit.push(edge_dest), visited.push(edge_dest);
        return false;
    }
    template<typename T, typename Edge_T> int component_of_graph::Vertex<T, Edge_T>::hash() const {
        int sum_hash_code{0};
        for (const auto& e : *this)
            sum_hash_code += e.hash();
        return Cont::hash() ^ hasher<T>{}(data) ^ hasher<int>{}(odeg()) ^ sum_hash_code;
    }

    template<typename vertex_data_T, typename edge_data_T> void Graph<vertex_data_T, edge_data_T>::update() const {
        for (auto& vertex : *this)
            const_cast<const Graph*&>(vertex.pgraph) = this;
    }
    template<typename vertex_data_T, typename edge_data_T> int Graph<vertex_data_T, edge_data_T>::cnt_edges() const {
        int cnt{0};
        for (const auto& vertex : *this)
            cnt += vertex.odeg();
        return cnt;
    }
    template<typename vertex_data_T, typename edge_data_T> Graph<vertex_data_T, edge_data_T>& Graph<vertex_data_T, edge_data_T>::reset(const bool clear_priority) const {
        for (auto& vertex : *this)
            vertex.reset();
        if (clk = 0; clear_priority) [[unlikely]]
            for (auto& vertex : *this)
                vertex.priority = ~Vector_v::npos;
        return const_cast<Graph&>(*this);
    }
    template<typename vertex_data_T, typename edge_data_T> Graph<vertex_data_T, edge_data_T>& Graph<vertex_data_T, edge_data_T>::undirect() const {
        for (const auto& origin : *this)
            for (static const constexpr auto shorter{[](const int a, const int b) -> int {
                     return a <= b ? a : b;
                 }};
                 auto& edge : origin)
                if (Vertex & dest{(*this)[edge.dest()]}; dest.has_edge_to(origin.rank()))
                    edge.dist = dest(origin.rank()).dist = shorter(edge.dist, dest(origin.rank()).dist);
                else
                    dest >> edge.dist >> origin;
        return const_cast<Graph&>(*this);
    }
    template<typename vertex_data_T, typename edge_data_T> Graph<vertex_data_T, edge_data_T>& Graph<vertex_data_T, edge_data_T>::push(vertex_data_T v_data) { return static_cast<Graph&>(Vector_v::push({v_data, size(), *this})); }
    template<typename vertex_data_T, typename edge_data_T> Graph<vertex_data_T, edge_data_T>::Vertex Graph<vertex_data_T, edge_data_T>::pick(const int i) {
        while ((*this)[i].odeg() != 0)
            (*this)[i].cut((*this)[i][0].dest());
        for (auto& vertex : *this) {
            vertex.parent > i ? --vertex.parent : 0, vertex.rank() > i ? --vertex._rank : 0;
            for (int lo{0}; lo < vertex.odeg(); ++lo)
                if (const int edge_dest{vertex[lo].dest()}; edge_dest == i)
                    vertex.cut(edge_dest), --lo;
                else
                    edge_dest > i ? --const_cast<int&>(edge_dest) : 0;
        }
        return Vector_v::pick(i);
    }
    template<typename vertex_data_T, typename edge_data_T> int Graph<vertex_data_T, edge_data_T>::find(const vertex_data_T& vertex_data, int lo, const int hi) const {
        for (Vector_v::npos_off(hi); lo < hi; ++lo)
            if ((*this)[lo].data == vertex_data)
                return lo;
        return Vector_v::npos;
    }
    template<typename vertex_data_T, typename edge_data_T> auto Graph<vertex_data_T, edge_data_T>::from(const int origin) const {
        struct _path final {
            Vector<int> to(int dest) const {
                Vector<int> path{};
                for (path.push(dest), dest = cref_graph[dest].parent; dest != Vector_v::npos; dest = cref_graph[dest].parent)
                    path.push(dest);
                if (path[path.size() - 1] == origin)
                    return path.reverse();
                else
                    return {};
            }
          private:
            const int origin;
            const Graph<vertex_data_T, edge_data_T>& cref_graph;
            _path(const int origin, const Graph<vertex_data_T, edge_data_T>& cref_graph): origin{origin}, cref_graph{cref_graph} {}
            friend class Graph<vertex_data_T, edge_data_T>;
        };
        return _path{origin, *this};
    }
    template<typename vertex_data_T, typename edge_data_T> bool Graph<vertex_data_T, edge_data_T>::operator<(const Graph& b) const { return this < &b; }
    template<typename vertex_data_T, typename edge_data_T> int Graph<vertex_data_T, edge_data_T>::hash() const {
        int sum_hash_code{0};
        for (const auto& v : *this)
            sum_hash_code += v.hash();
        return Cont::hash() ^ sum_hash_code;
    }

    template<typename vertex_data_T, typename edge_data_T> template<typename F> F& Graph<vertex_data_T, edge_data_T>::BFS(const F& cref_f, const int u) const {
        return search(
            cref_f, [this](F& f, const int u) {
                this->breadth_first_search(f, u);
            },
            u);
    }
    template<typename vertex_data_T, typename edge_data_T> template<typename F> F& Graph<vertex_data_T, edge_data_T>::DFS(const F& cref_f, const int u) const {
        return search(
            cref_f, [this](F& f, const int u) {
                this->depth_first_search(f, u);
            },
            u);
    }
    template<typename vertex_data_T, typename edge_data_T> template<typename Priority_updater_T> Priority_updater_T& Graph<vertex_data_T, edge_data_T>::PFS(const Priority_updater_T& cref_pu, const int u) const {
        return search(
            cref_pu, [this](Priority_updater_T& pu, const int u) {
                this->priority_first_search(pu, u);
            },
            u);
    }

    template<typename vertex_data_T, typename edge_data_T> template<typename F, typename XFS> F& Graph<vertex_data_T, edge_data_T>::search(const F& cref_f, const XFS x_first_search, int u) const {
        F& f{const_cast<F&>(cref_f)};
        if (size() == 0)
            return f;
        else if (u == Vector_v::npos)
            for (u = 0, reset(); u < size(); ++u)
                if ((*this)[u].state == UNDISCOVERED)
                    x_first_search(f, u);
                else
                    continue;
        else
            (*this)[u].state == UNDISCOVERED ? x_first_search(f, u) : []() {}();
        return f;
    }

    template<typename vertex_data_T, typename edge_data_T> template<typename F> void Graph<vertex_data_T, edge_data_T>::breadth_first_search(F& f, const int u) const {
        List<int> queue{};
        for ((*this)[u].state = DISCOVERED, (*this)[u].tag1 = clk++, f((*this)[u].data), queue.push(u); queue.size() != 0;)
            for (int v{queue.deque()}; auto& edge : (*this)[v])
                if (Vertex & dest{(*this)[edge.dest()]}; dest.state != UNDISCOVERED)
                    edge.type = CROSS;
                else
                    dest.parent = v, edge.type = TREE, dest.state = DISCOVERED, dest.tag1 = clk++, f(dest.data), queue.push(dest.rank());
        (*this)[u].state = VISITED;
    }
    template<typename vertex_data_T, typename edge_data_T> template<typename F> void Graph<vertex_data_T, edge_data_T>::depth_first_search(F& f, const int u) const {
        for ((*this)[u].state = DISCOVERED, (*this)[u].tag1 = clk++, f((*this)[u].data); auto& edge : (*this)[u])
            switch (Vertex & v{(*this)[edge.dest()]}; v.state) {
                case VISITED:
                    edge.type = v.tag1 > (*this)[u].tag1 ? FORWARD : CROSS;
                    break;
                case DISCOVERED:
                    edge.type = BACKWARD;
                    break;
                case UNDISCOVERED:
                    v.parent = u, edge.type = TREE, depth_first_search(f, v.rank());
                    break;
            }
        (*this)[u].state = VISITED, (*this)[u].tag2 = clk++;
    }
    template<typename vertex_data_T, typename edge_data_T> template<typename Priority_updater_T> void Graph<vertex_data_T, edge_data_T>::priority_first_search(Priority_updater_T& pu, int u) const {
        for (int& v{((*this)[u].state = VISITED, (*this)[u].tag1 = clk++, (*this)[u].parent = Vector_v::npos, (*this)[u].priority = 0, u)};; (*this)[(*this)[v].parent](v).type = TREE, (*this)[v].state = VISITED, (*this)[v].tag1 = clk++) {
            for (const auto& edge : (*this)[u])
                pu(u, edge.dest());
            for (int best_val{~Vector_v::npos}; const auto& vertex : *this)
                if (vertex.state == UNDISCOVERED && vertex.priority < best_val)
                    v = vertex.rank(), best_val = vertex.priority;
            if ((*this)[v].state == VISITED)
                break;
        }
    }

    template<typename vertex_data_T, typename edge_data_T> Vector<auto> Graph<vertex_data_T, edge_data_T>::Prim(const int u) const {
        const auto prim_alg{[this](const int u, const int v) {
            if (Vertex & dest{(*this)[v]}; dest.state == UNDISCOVERED && (*this)[u](v).dist < dest.priority)
                dest.priority = (*this)[u](v).dist, dest.parent = u;
        }};
        undirect().reset(true).PFS(prim_alg, u);
        struct simple_edge final {
            const int origin, dest;
        };
        Vector<simple_edge> edges{};
        if (List<int> vertices{}; size() != 0)
            for (vertices.push(u); vertices.size() != 0; vertices.deque())
                for (const Vertex& u{(*this)[vertices[0]]}; const auto& edge : u)
                    if (edge.type == TREE)
                        edges.push({u.rank(), edge.dest()}), vertices.push(edge.dest());
        return edges;
    }
    template<typename vertex_data_T, typename edge_data_T> Vector<int> Graph<vertex_data_T, edge_data_T>::Dijkstra(const int u) const {
        const auto dijkstra_alg{[this](const int u, const int v) {
            if (const int new_len{(*this)[u].priority + (*this)[u](v).dist}; (*this)[v].state == UNDISCOVERED && new_len < (*this)[v].priority)
                (*this)[v].parent = u, (*this)[v].priority = new_len;
        }};
        Vector<int> reachable{};
        if (List<int> spots{}; (reset(true).PFS(dijkstra_alg, u), size()) != 0)
            for (reachable.push(u), spots.push(u); spots.size() != 0; spots.deque())
                for (const Vertex& origin{(*this)[spots[0]]}; const auto& edge : origin)
                    if (const int dest{edge.dest()}; edge.type == TREE)
                        reachable.push(dest), spots.push(dest);
        return reachable;
    }
    template<typename vertex_data_T, typename edge_data_T> Vector<int> Graph<vertex_data_T, edge_data_T>::topo_sort(const int v) const {
        const struct {
            const Graph& cref_graph;
            mutable Vector<int> reversed;
            void operator()(Vertex& u) const {
                for (u.state = DISCOVERED; const auto& edge : u)
                    if (auto& dest{cref_graph[edge.dest()]}; dest.state == UNDISCOVERED)
                        (*this)(dest);
                u.state = VISITED, reversed.push(u.rank());
            }
        } dfs{*this};
        for (reset(); const auto& vertex : *this)
            if (vertex.cyclic())
                DFS([](const auto&) {}, vertex.rank());
        for (auto& vertex : *this)
            if (vertex.state == UNDISCOVERED)
                dfs(vertex);
        if (bool simplified{true}; v != Vector_v::npos) {
            for (int lo{0}; lo < dfs.reversed.size(); ++lo)
                if (const auto& vertex{(*this)[(dfs.reversed)[lo]]}; vertex.odeg() == 0 && vertex.rank() != v)
                    dfs.reversed.pick(lo--);
            do {
                for (int lo{(reset(), 0)}; lo < dfs.reversed.size(); ++lo, reset())
                    if (DFS([](const auto&) {}, (dfs.reversed)[lo]); (*this)[v].state == UNDISCOVERED)
                        simplified = false, dfs.reversed.pick(lo--);
                simplified = true;
            } while (!simplified);
        }
        return static_cast<Vector<int>&&>(dfs.reversed.reverse());
    }
    template<typename vertex_data_T, typename edge_data_T> Vector<Vector<int>> Graph<vertex_data_T, edge_data_T>::bcc() const {
        const struct {
            const Graph& cref_g;
            int& clk;
            int Vertex::*const p_highest_connected_ancestor = &Vertex::tag2;
            mutable Vector<Vector<int>> components;
            mutable Vector<int> vertices;
            void operator()(Vertex& u) const {
                for (u.tag1 = clk++, u.state = DISCOVERED, vertices.push(u.rank()); const auto& edge : u) {
                    switch (auto& v{cref_g[edge.dest()]}; v.state) {
                        case UNDISCOVERED:
                            if (v.*p_highest_connected_ancestor = u.tag1, (*this)(v); v.*p_highest_connected_ancestor == u.tag1)
                                decompose(u.rank());
                            else
                                v.*p_highest_connected_ancestor < u.*p_highest_connected_ancestor ? (u.*p_highest_connected_ancestor = v.*p_highest_connected_ancestor) : 0;
                            break;
                        default:
                            v.tag1 < u.*p_highest_connected_ancestor ? (u.*p_highest_connected_ancestor = v.tag1) : 0;
                            break;
                    }
                }
            }
            void decompose(const int v) const {
                Vector<int> few_vertices{};
                do
                    few_vertices.push(vertices.pop());
                while (few_vertices[few_vertices.size() - 1] != v);
                vertices.push(v), components.push(static_cast<Vector<int>&&>(few_vertices));
            }
        } dfs{undirect().reset(), clk};
        for (auto& vertex : *this)
            if (clk = 0; vertex.state == UNDISCOVERED)
                vertex.odeg() == 0 ? (dfs.components.push(Vector<int>{}.push(vertex.rank())), vertex.state = DISCOVERED) : (vertex.tag2 = Vector_v::npos, dfs(vertex), 0);
        return static_cast<Vector<Vector<int>>&&>(dfs.components);
    }

} // namespace conts
namespace conts {
    /* impl for List */

    template<typename T> auto& List<T>::_Node::insert_at(_Node& inserted_at) {
        if (_Node _{nullptr, nullptr}; this == &inserted_at && pright == &inserted_at)
            return static_cast<Node&>(*this);
        else
            return (new (&_) _Node{pleft, pright})->~auto(), (new (&_) _Node{inserted_at.pleft, this})->~auto(), (new (&_) _Node{this, &inserted_at})->~auto(), static_cast<Node&>(*this);
    }
    template<typename T> T& List<T>::_Node::data() { return static_cast<Node&>(*this)._data; }

    template<typename T> List<T>::~List() { Sequential::destructor(); }

    template<typename T> List<T>::_Node& List<T>::at(const int i) const {
        if (static const constexpr int divisor{4}; i <= size() / divisor)
            return *(decltype(vernier){-1, &const_cast<_Node&>(head)}.go(i + 1).p);
        else if (size() / divisor <= i)
            return *(decltype(vernier){size(), &const_cast<_Node&>(tail)}.go(i - size()).p);
        else
            return *(vernier.go(i - vernier.rank).p);
    }
    template<typename T> List<T>& List<T>::dsize(const int det) { return dynamic_cast<List&>(Sequential::dsize(det)); }
    template<typename T> List<T>& List<T>::operator=(List b) { return swap(*this, b), *this; }

    template<typename T> T& List<T>::operator[](const int i) const noexcept(false&& noexcept(Sequential::operator[])) { return (at((0 <= i && i < size()) ? i : throw "List_not[0,size)")).data(); }
    template<typename T> List<T>& List<T>::push(T e) { return vernier.rank < size() ? vernier : (vernier = {size(), &tail}).go(-1), new Node{at(size() - 1), static_cast<T&&>(e), tail}, dsize(1); }
    template<typename T> T List<T>::pop() noexcept(noexcept(operator[](0))) {
        T to_pop{static_cast<T&&>((*this)[size() - 1])};
        vernier.rank < (size() - 1) ? vernier : (vernier = {size(), &tail}).go(-2), delete dsize(-1).tail.pleft;
        return to_pop;
    }
    template<typename T> T List<T>::deque() noexcept(noexcept(operator[](0))) {
        T to_dequeue{static_cast<T&&>((*this)[0])};
        --(0 < vernier.rank ? vernier : (vernier = {-1, &head}).go(2)).rank, delete dsize(-1).head.pright;
        return to_dequeue;
    }

    template<typename T> bool List<T>::operator<(const List& b) const { return *this < static_cast<const Sequential&>(b); }

    template<typename T> List<T>& List<T>::insert(const int i, T e) { return insert(at(i), static_cast<T&&>(e), i); }
    template<typename T> List<T>& List<T>::insert(_Node& node_i, T e, const int i) { return i == npos ? (vernier = {-1, &head}).rank : (vernier.rank < i ? 0 : ++vernier.rank), new Node{*(node_i.pleft), static_cast<T&&>(e), node_i}, dsize(1); }
    template<typename T> T List<T>::pick(const int i) { return pick(at(i), i); }
    template<typename T> T List<T>::pick(_Node& node_i, const int i) {
        T to_pick{static_cast<T&&>(node_i.data())};
        return i == npos ? (vernier = {-1, &head}).rank : (vernier.rank < i ? 0 : (vernier.rank == i ? vernier.go(-1).rank : --vernier.rank)), delete &node_i, static_cast<T&&>(to_pick);
    }

    template<typename T> void List<T>::merge_sort(const int lo, const int hi) const {
        if (_Node & ref_lo{at(lo)}; hi - lo <= 1)
            return;
        else
            merge_sort(lo, lo, (lo + hi) / 2, hi, ref_lo.pleft, &ref_lo, &ref_lo, &at(hi)), [&]() {
                vernier = {-1, &const_cast<_Node&>(head)};
            }();
    }
    template<typename T> void List<T>::merge_sort(int lo, int mi, const int cmi, const int chi, _Node* plo_left, _Node* plo, _Node* pmi, _Node* const cphi) const {
        if (chi - lo <= 1)
            return;
        else {
            while (mi++ != cmi)
                pmi = pmi->pright;
            merge_sort(lo, lo, (lo + cmi) / 2, cmi, plo_left, plo, plo, pmi), merge_sort(cmi, cmi, (cmi + chi) / 2, chi, pmi->pleft, pmi, pmi, cphi);
            for (mi = lo, pmi = plo = plo_left->pright; mi != cmi; ++mi)
                pmi = pmi->pright;
            while (lo != cmi && mi != chi)
                if (plo->data() <= pmi->data())
                    plo = plo->pright, ++lo;
                else
                    (pmi = pmi->pright)->pleft->insert_at(*plo), ++mi;
        }
    }

} // namespace conts
namespace conts {
    /* impl for slice */

    template<typename T> slice<T>& slice<T>::operator=(slice b) { return swap(*this, b), *this; }
    template<typename T> slice<T>& slice<T>::operator+=(slice b) {
        const int this_old_size{Vector_p::size()};
        for (int hi{b.size()}; 0 != hi; --hi)
            Vector_p::push(b.pop());
        return static_cast<slice&>(Vector_p::reverse(this_old_size, Vector_p::size()));
    }
    template<typename T> bool slice<T>::operator<(const slice& b) const { return *this < static_cast<const Vector_p&>(b); }
    template<typename T> auto_deref::AutoDeref<T>& slice<T>::operator[](const int i) const noexcept(noexcept(Vector_p::operator[])) { return Vector_p::operator[](i); }

} // namespace conts
namespace conts {
    /* impl for Vector */

    template<typename T> Vector<T>& Vector<T>::dsize(const int det) { return dynamic_cast<Vector&>(Sequential::dsize(det)); }
    template<typename T> Vector<T>& Vector<T>::destruct(const int lo, const int hi) const {
        for (int dec_hi{npos_off(hi, lo + 1) - 1}; lo <= dec_hi; --dec_hi)
            (*this)[dec_hi].~T();
        Block::clr(lo != hi ? &(*this)[lo] : nullptr, hi - lo);
        return *const_cast<Vector*>(this);
    }
    template<typename T> Vector<T>& Vector<T>::scale() {
        if ((!(capacity / 4 <= size() && size() < capacity)) && capacity != 0) {
            T* new_elem{size() == capacity ? *(new Block[capacity *= 2]) : *(new Block[capacity /= 2])};
            for (int lo{0}, hi{size()}; lo < hi; ++lo)
                new (new_elem + lo) T{static_cast<T&&>((*this)[lo])};
            Block::del(destruct(0, size()).elem) = new_elem;
        }
        return *this;
    }

    template<typename T> Vector<T>& Vector<T>::operator=(Vector b) { return swap(*this, b), *this; }
    template<typename T> Vector<T>::~Vector<T>() { Block::del(destruct(capacity = 0, size()).elem); }

    template<typename T> T& Vector<T>::operator[](const int i) const noexcept(false&& noexcept(Sequential::operator[])) { return elem[(0 <= i && i < size()) ? i : throw "Vector_[size,+infinity)"]; }
    template<typename T> Vector<T>& Vector<T>::push(T e) {
        scale(), new (&dsize(1)[size() - 1]) T{static_cast<T&&>(e)};
        return *this;
    }
    template<typename T> T Vector<T>::pop() noexcept(noexcept(operator[](0))) {
        T to_pop{static_cast<T&&>((*this)[size() - 1])};
        destruct(size() - 1).dsize(-1).scale();
        return to_pop;
    }

    template<typename T> bool Vector<T>::operator<(const Vector& b) const { return *this < static_cast<const Sequential&>(b); }

    template<typename T> Vector<T>& Vector<T>::operator+=(Vector b) {
        const int this_old_size{size()};
        for (int hi{b.size()}; 0 != hi; --hi)
            push(b.pop());
        return dynamic_cast<Vector&>(Sequential::reverse(this_old_size, size()));
    }
    template<typename T> Vector<T>::operator const T*() const { return (const_cast<Vector*>(this)->scale()).elem; }

    template<typename T> Vector<T>& Vector<T>::insert(const int i, T e) { return dynamic_cast<Vector&>(Sequential::insert(i, static_cast<T&&>(e))); }
    template<typename T> Vector<T>& Vector<T>::pick(const int lo, const int hi) { return dynamic_cast<Vector&>(Sequential::pick(lo, hi)); }
    template<typename T> Vector<T>& Vector<T>::shuffle(const int lo, const int hi) const { return dynamic_cast<Vector&>(Sequential::shuffle(lo, hi)); }
    template<typename T> Vector<T>& Vector<T>::reverse(const int lo, const int hi) const { return dynamic_cast<Vector&>(Sequential::reverse(lo, hi)); }
    template<typename T> Vector<T>& Vector<T>::sort(const int lo, const int hi) const { return sort(nullptr, lo, hi); }
    template<typename T> Vector<T>& Vector<T>::sort(const char* const which, const int lo, const int hi) const { return dynamic_cast<Vector&>(Sequential::sort(which, lo, hi)); }
    template<typename T> void Vector<T>::heap_sort(const int lo, const int hi) const {
        if (Vector & this_vec{const_cast<Vector&>(*this)}; lo != 0 || hi != size())
            Sequential::heap_sort(lo, hi);
        else {
            CHeap<T> heap{static_cast<Vector&&>(this_vec)};
            for (this_vec = {}; heap.size() != 0;)
                this_vec.push(heap.deque());
        }
    }

    template<> String::Vector(const char* s): Sequential{}, elem{*(new Block[1])}, capacity{1} {
        if (s)
            while (*s)
                push(*s++);
    }
    template<> uString::Vector(const char* s): Sequential{}, elem{*(new Block[1])}, capacity{1} {
        if (s)
            while (*s)
                push(*s++);
    }

    template<> class Vector<bool>: private virtual Vector<unsigned char> {
        using uString::operator[];
        using uString::operator<;
        using uString::pick;
        struct iterator {
            bool operator!=(const iterator& b) const;
            bool operator==(const iterator& b) const;
            iterator& operator++();
            auto operator*() const;
          private:
            BitMap& bmp;
            int i;
            friend class Vector<bool>;
            iterator(const BitMap& bmp, const int i): bmp{const_cast<BitMap&>(bmp)}, i{i} {}
        };
      public:
        Vector() = default;
        Vector(const BitMap& b): uString{static_cast<const uString&>(b)} {}
        Vector(BitMap&& b) noexcept: uString{static_cast<uString&&>(b)} {}
        friend void swap(BitMap& a, BitMap& b) { swap(static_cast<uString&>(a), static_cast<uString&>(b)); }
        BitMap& operator=(BitMap b) { return swap(*this, b), *this; }
        virtual ~Vector() override = default;

        Vector(const String& bit_sequen);
        Vector(const char* const bit_sequen);
        Vector(const unsigned char* const bit_sequen);
        Vector(int number);

        using uString::size;
        auto operator[](const int i);
        using uString::operator const unsigned char*;
        bool operator<(const BitMap& b) const;

        iterator begin() const;
        iterator end() const;
        virtual int hash() const override;

        BitMap& pick(const int lo, int hi = uString::npos);
        BitMap& operator<<(const String fname);
        BitMap& operator>>(String fname);

        int cnt_ones(int lo = 0, const int hi = uString::npos) const;
        int cnt_bits() const;
        int highest() const;
        int as_int() const;
    };

} // namespace conts
namespace conts {
    /* impl for BitMap */

    bool BitMap::iterator::operator!=(const iterator& b) const { return i != b.i; }
    bool BitMap::iterator::operator==(const iterator& b) const { return !(*this != b); }
    BitMap::iterator& BitMap::iterator::operator++() {
        ++i;
        return *this;
    }

    auto BitMap::operator[](const int i) {
        for (int this_size{size() * 8}; this_size <= i; this_size += 8)
            uString::push(0);
        struct bit_setter;
        void swap(const bit_setter&, const bit_setter&);
        struct bit_setter final {
            const bit_setter& operator=(const bool bit) const {
                static constexpr const unsigned char carries[]{0b0000'0001, 0b0000'0010, 0b0000'0100, 0b0000'1000, 0b0001'0000, 0b0010'0000, 0b0100'0000, 0b1000'0000};
                if (unsigned char& byte{bytes[i / 8]}; bit)
                    byte |= carries[i % 8];
                else
                    byte &= ~carries[i % 8];
                return *this;
            }
            operator bool() const {
                static constexpr const unsigned char carries[]{0b0000'0001, 0b0000'0010, 0b0000'0100, 0b0000'1000, 0b0001'0000, 0b0010'0000, 0b0100'0000, 0b1000'0000};
                return bytes[i / 8] & carries[i % 8];
            }
            const bit_setter& operator=(const bit_setter& b) const { return *this = static_cast<bool>(b); }
            bool operator<(const bit_setter& b) const { return static_cast<bool>(*this) < static_cast<bool>(b); }
            friend void swap(const bit_setter&, const bit_setter&);
          private:
            const uString& bytes;
            const int i;
            friend class Vector<bool>;
            bit_setter(const uString& bytes, const int i): bytes{bytes}, i{i} {}
        };
        return bit_setter{static_cast<const uString&>(*this), i};
    }
    auto BitMap::iterator::operator*() const { return bmp[i]; }
    void swap(const decltype(BitMap{}[0])& a, const decltype(BitMap{}[0])& b) {
        const bool boolean_a{a}, boolean_b{b};
        a = boolean_b, b = boolean_a;
    }
    bool BitMap::operator<(const BitMap& b) const {
        for (int lo{0}, shorter_len{size() <= b.size() ? size() : b.size()}; lo < shorter_len; ++lo)
            if (const bool b_lo{const_cast<BitMap&>(b)[lo]}; const_cast<BitMap&>(*this)[lo] != b_lo)
                return b_lo;
        return size() < b.size();
    }

    BitMap::Vector(const String& bit_sequen): BitMap{} {
        for (int i{0}, cnt{bit_sequen.size()}; i < cnt; ++i)
            const_cast<BitMap&>(*this)[i] = (bit_sequen[i] != '0');
    }
    BitMap::Vector(int number): BitMap{} {
        for (int i{0}; number != 0; (number >>= 1) &= ~npos)
            const_cast<BitMap&>(*this)[i++] = number & 1;
    }
    BitMap::Vector(const char* const bit_sequen): BitMap{static_cast<const String&>(bit_sequen)} {}
    BitMap::Vector(const unsigned char* const bit_sequen): BitMap{static_cast<const String&>(bit_sequen)} {}

    BitMap::iterator BitMap::begin() const { return {*this, 0}; }
    BitMap::iterator BitMap::end() const { return {*this, size() * 8}; }
    int BitMap::hash() const { return Cont::hash() ^ as_int(); }

    BitMap& BitMap::pick(const int lo, int hi) {
        for (--uString::npos_off(hi); lo <= hi; --hi)
            uString::pick(hi);
        return *this;
    }

    BitMap& BitMap::operator<<(const String fname) {
        if (std::ifstream fin{fname, std::ifstream::in | std::ifstream::binary}; fin) {
            const int old_length{size()}, file_length = std::filesystem::file_size(static_cast<const char*>(fname));
            const_cast<BitMap&>(*this)[(size() + file_length) * 8 - 1];
            fin.read(reinterpret_cast<char*>(const_cast<unsigned char*>(static_cast<const unsigned char*>(*this))) + old_length, file_length);
        }
        return *this;
    }
    BitMap& BitMap::operator>>(String fname) {
        for (int name_len{fname.size()}, hi{name_len - 1}; -1 <= hi; --hi)
            if (hi == -1)
                fname.insert(name_len, 'r').insert(name_len, 'u').insert(name_len, 'n').insert(name_len, 'y').insert(name_len, 'h').insert(name_len, 'S').insert(name_len, '.');
            else if (fname[hi] == '.')
                fname.insert(hi, 'r').insert(hi, 'u').insert(hi, 'n').insert(hi, 'y').insert(hi, 'h').insert(hi, 'S').insert((name_len = hi, hi = npos + 1, name_len), '.');
            else
                continue;
        std::ofstream fout{fname, std::ofstream::out | std::ofstream::binary};
        fout.write(reinterpret_cast<char*>(const_cast<unsigned char*>(static_cast<const unsigned char*>(*this))), size());
        return *this;
    }

    int BitMap::cnt_ones(int lo, const int hi) const {
        int cnt{0};
        for (npos_off(hi, size() * 8 - 1); lo < hi; ++lo)
            const_cast<BitMap&>(*this)[lo] ? ++cnt : 0;
        return cnt;
    }
    int BitMap::cnt_bits() const { return 8 * size(); }
    int BitMap::highest() const {
        for (int dec_hi{size() * 8 - 1}; dec_hi >= 0; --dec_hi)
            if (const_cast<BitMap&>(*this)[dec_hi])
                return dec_hi;
        return npos;
    }
    int BitMap::as_int() const {
        int number{0};
        for (int dec_hi{highest()}; dec_hi >= 0; --dec_hi)
            (number <<= 1) += const_cast<BitMap&>(*this)[dec_hi] ? 1 : 0;
        return number;
    }

} // namespace conts
namespace conts {
    /* impl for Sequential */

    template<typename T> void Sequential<T>::copy_constructor(const Sequential& b, int lo, const int hi) {
        if constexpr (requires(const T& copyable) { new T{copyable}; })
            for (b.npos_off(hi); lo < hi; ++lo)
                push(b[lo]);
    }
    template<typename T> void Sequential<T>::move_constructor(Sequential&& _b) noexcept { swap(*this, static_cast<Sequential&>(_b)); }
    template<typename T> void Sequential<T>::destructor() { pick(0, size()); }
    template<typename T> Sequential<T>& Sequential<T>::dsize(const int det) { return dynamic_cast<Sequential&>(Cont::dsize(det)); }

    template<typename T> bool Sequential<T>::operator<(const Cont& b) const {
        const Sequential* const pb{dynamic_cast<const Sequential*>(&b)};
        return pb != nullptr ? (*this < *pb) : Cont::operator<(b);
    }
    template<typename T> bool Sequential<T>::operator<(const Sequential& b) const {
        for (int i{0}; i < size() && i < b.size(); ++i)
            if ((*this)[i] != b[i])
                return (*this)[i] < b[i];
        return Cont::operator<(b);
    }

    template<typename T> Sequential<T>& Sequential<T>::insert(int i, T e) {
        i <= size() ? push(static_cast<T&&>(e)) : throw "insert in (size,+infinity)";
        while (i < size())
            swap((*this)[i++], (*this)[size() - 1]);
        return *this;
    }
    template<typename T> T Sequential<T>::pick(const int i) {
        T to_pick{static_cast<T&&>((*this)[i])};
        pick(i, i + 1);
        return to_pick;
    }
    template<typename T> Sequential<T>& Sequential<T>::pick(const int lo, int hi) {
        for (int i{hi}; i < size(); ++i)
            swap((*this)[i], (*this)[i - (hi - lo)]);
        while (lo < hi--)
            pop();
        return *this;
    }
    template<typename T> template<typename F> F& Sequential<T>::traverse(const F& cref_f, int lo, const int hi) const noexcept(noexcept(F::operator())) {
        F& f{const_cast<F&>(cref_f)};
        while (lo < npos_off(hi))
            f((*this)[lo++]);
        return f;
    }

    template<typename T> int Sequential<T>::count(const T& e, const int lo, int hi) const {
        int cnt{-1};
        return traverse([&cnt, &e](const T& _e) -> int {
            return e == _e ? ++cnt : 0;
        },
                        lo, hi)(e);
    }
    template<typename T> T& Sequential<T>::major(int lo, const int hi) const {
        const T* candidate;
        for (candidate = &(*this)[(npos_off(hi), lo++)]; lo < hi; lo != hi ? (candidate = &(*this)[lo++]) : nullptr)
            for (int det{1}; det != 0 && lo != hi; ++lo)
                (*this)[lo] == *candidate ? ++det : --det;
        return *const_cast<T*>(candidate);
    }
    template<typename T> Sequential<T>& Sequential<T>::shuffle(const int lo, const int hi) const {
        for (int i{(npos_off(hi), lo)}; i < hi; ++i)
            swap((*this)[i], (*this)[i + rand(hi - i)]);
        return const_cast<Sequential&>(*this);
    }
    template<typename T> Sequential<T>& Sequential<T>::reverse(int lo, int hi) const {
        for (--npos_off(hi); lo < hi; ++lo, --hi)
            swap((*this)[lo], (*this)[hi]);
        return const_cast<Sequential&>(*this);
    }
    template<typename T> bool Sequential<T>::sorted(int lo, const int hi) const {
        for (int& dec_hi{--npos_off(hi)}; lo < dec_hi; ++lo)
            if ((*this)[lo] > (*this)[lo + 1])
                return false;
        return true;
    }

    template<typename T> int Sequential<T>::find(const T& e, int lo, const int hi) const {
        for (npos_off(hi); lo < hi; ++lo)
            if ((*this)[lo] == e)
                return lo;
        return npos;
    }
    template<typename T> int Sequential<T>::qfind(const T& e) const {
        const struct {
            const Sequential& this_sequential;
            const T& target;
            int operator()(const int lo, const int hi) const { return (hi - lo <= 1) ? ((hi - lo == 1 && target < this_sequential[lo]) ? lo : hi) : (this_sequential[(lo + hi) / 2] <= target ? (*this)((lo + hi) / 2, hi) : (*this)(lo, (lo + hi) / 2)); }
        } bin_searcher{*this, e};
        return bin_searcher(0, sort("quick").size());
    }
    template<typename T> int Sequential<T>::dedup(const int lo, int hi) {
        if (npos_off(hi); sorted(lo, hi))
            return qdedup(lo, hi);
        int old_size{size()};
        for (--hi; lo < hi; --hi)
            if (find((*this)[hi], lo, hi) != npos)
                pick(hi);
        return old_size - size();
    }
    template<typename T> int Sequential<T>::qdedup(const int lo, int hi) {
        int old_size{size()}, cnt_unique{1};
        for (int i{lo + 1}; i < hi; ++i)
            if ((*this)[i] != (*this)[lo + cnt_unique - 1])
                swap((*this)[lo + cnt_unique++], (*this)[i]);
        while (lo + cnt_unique < hi)
            pick(--hi);
        return old_size - size();
    }

    template<typename T> Sequential<T>& Sequential<T>::sort(const int lo, const int hi) const { return sort(nullptr, lo, hi); }
    template<typename T> Sequential<T>& Sequential<T>::sort(const char* const which, const int lo, const int hi) const {
        if (npos_off(hi); hi - lo <= 1) [[unlikely]]
            return const_cast<Sequential&>(*this);
        else
            switch (option(which, "insertion", "bubble", "merge", "quick", "selection", "Shell", "heap")) {
                case 1:
                    insertion_sort(lo, hi);
                    break;
                case 2:
                    bubble_sort(lo, hi);
                    break;
                case 3:
                    merge_sort(lo, hi);
                    break;
                case 4:
                    quick_sort(lo, hi);
                    break;
                case 5:
                    selection_sort(lo, hi);
                    break;
                case 6:
                    Shell_sort(lo, hi);
                    break;
                case 7:
                    [[likely]] default: heap_sort(lo, hi);
            }
        return const_cast<Sequential&>(*this);
    }
    template<typename T> void Sequential<T>::bubble_sort(const int lo, int hi) const {
        for (bool already_sorted{(--hi, false)}; already_sorted == false; --hi)
            for (int i{(already_sorted = true, lo)}; i < hi; ++i)
                (*this)[i] <= (*this)[i + 1] ? 0 : (swap((*this)[i], (*this)[i + 1]), already_sorted = false);
    }
    template<typename T> void Sequential<T>::merge_sort(int lo, const int hi) const {
        if (static const constexpr auto _f{[]() {}}; hi - lo <= 2)
            return (hi - lo == 2 && (*this)[lo] > (*this)[lo + 1]) ? swap((*this)[lo], (*this)[lo + 1]) : _f();
        int mi{(lo + hi) / 2};
        const auto hand_shaker{(merge_sort(lo, mi), merge_sort(mi, hi), [this](int& lo, int& mi, const int hi) {
            (*this).reverse(lo, mi).reverse(mi, hi).reverse(lo, hi);
            lo += hi - mi, mi = hi;
        })};
        for (int _mi{mi}; lo < mi && mi < hi; hand_shaker(lo, _mi, mi)) {
            while (lo < mi && (*this)[lo] <= (*this)[mi])
                if (++lo == mi)
                    return;
            while (mi < hi && (*this)[mi] < (*this)[lo])
                ++mi;
        }
    }
    template<typename T> void Sequential<T>::quick_sort(const int lo, const int hi) const {
        if (int pivot{lo}; hi - lo <= 1)
            return;
        else
            swap((*this)[lo], (*this)[lo + rand(hi - lo)]), quick_pivot(pivot, hi), quick_sort(lo, pivot), quick_sort(pivot + 1, hi);
    }
    template<typename T> void Sequential<T>::insertion_sort(const int lo, const int hi, const int det) const {
        const auto inserter_in_prefix{[this, det](int dest, const int src) {
            while (dest < src)
                swap((*this)[dest], (*this)[src]), dest += det;
        }};
        const auto qfinder{[this, det](const T& e, int lo, int hi) -> int {
            while (lo <= hi)
                if (int mi{lo + (hi - lo) / det / 2 * det}; lo + det < hi)
                    (*this)[mi] <= e ? (lo = mi) : (hi = mi);
                else
                    return (lo + det == hi && e < (*this)[lo]) ? lo : hi;
            return 0;
        }};
        for (int i{lo + det}; i < hi; i += det)
            inserter_in_prefix(qfinder((*this)[i], lo, i), i);
    }
    template<typename T> void Sequential<T>::Shell_sort(const int lo, const int hi) const { /* (2^k); (2^k - 1);  (2^p * 2^q); (9 * 4^n - 9 * 2^n + 1) or (4^n - 3 * 2^n + 1); */
        static constexpr const int Shell[]{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 1'6384, 3'2768, 6'5536, 13'1072, 26'2144, 52'4288, 104'8576, 209'7152, 419'4304, 838'8608, 1677'7216, 3355'4432, 6710'8864, 1'3421'7728, 2'6843'5456, 5'3687'0912}, Papernov_Stasevic[]{1, 3, 7, 15, 31, 63, 127, 255, 511, 1023, 2047, 4095, 8191, 1'6383, 3'2767, 6'5535, 13'1071, 26'2143, 52'4287, 104'8575, 209'7151, 419'4303, 838'8607, 1677'7215, 3355'4431, 6710'8863, 1'3421'7727, 2'6843'5455, 5'3687'0911}, Pratt[]{1, 2, 3, 4, 6, 8, 9, 12, 16, 18, 24, 27, 32, 36, 48, 54, 64, 72, 81, 96, 108, 128, 144, 162, 192, 216, 243, 256, 288, 324, 384, 432, 486, 512, 576, 648, 729, 768, 864, 972, 1024, 1152, 1296, 1458, 1536, 1728, 1944, 2048, 2187, 2304, 2592, 2916, 3072, 3456, 3888, 4096, 4374, 4608, 5184, 5832, 6144, 6561, 6912, 7776, 8192, 8748, 9216, 1'0368, 1'1664, 1'2288, 1'3122, 1'3824, 1'5552, 1'6384, 1'7496, 1'8432, 1'9683, 2'0736, 2'3328, 2'4576, 2'6244, 2'7648, 3'1104, 3'2768, 3'4992, 3'6864, 3'9366, 4'1472, 4'6656, 4'9152, 5'2488, 5'5296, 5'9049, 6'2208, 6'5536, 6'9984, 7'3728, 7'8732, 8'2944, 9'3312, 9'8304, 10'4976, 11'0592, 11'8098, 12'4416, 13'1072, 13'9968, 14'7456, 15'7464, 16'5888, 17'7147, 18'6624, 19'6608, 20'9952, 22'1184, 23'6196, 24'8832, 26'2144, 27'9936, 29'4912, 31'4928, 33'1776, 35'4294, 37'3248, 39'3216, 41'9904, 44'2368, 47'2392, 49'7664, 52'4288, 53'1441, 55'9872, 58'9824, 62'9856, 66'3552, 70'8588, 74'6496, 78'6432, 83'9808, 88'4736, 94'4784, 99'5328, 104'8576, 106'2882, 111'9744, 117'9648, 125'9712, 132'7104, 141'7176, 149'2992, 157'2864, 159'4323, 167'9616, 176'9472, 188'9568, 199'0656, 209'7152, 212'5764, 223'9488, 235'9296, 251'9424, 265'4208, 283'4352, 298'5984, 314'5728, 318'8646, 335'9232, 353'8944, 377'9136, 398'1312, 419'4304, 425'1528, 447'8976, 471'8592, 478'2969, 503'8848, 530'8416, 566'8704, 597'1968, 629'1456, 637'7292, 671'8464, 707'7888, 755'8272, 796'2624, 838'8608, 850'3056, 895'7952, 943'7184, 956'5938, 1007'7696, 1061'6832, 1133'7408, 1194'3936, 1258'2912, 1275'4584, 1343'6928, 1415'5776, 1434'8907, 1511'6544, 1592'5248, 1677'7216, 1700'6112, 1791'5904, 1887'4368, 1913'1876, 2015'5392, 2123'3664, 2267'4816, 2388'7872, 2516'5824, 2550'9168, 2687'3856, 2831'1552, 2869'7814, 3023'3088, 3185'0496, 3355'4432, 3401'2224, 3583'1808, 3774'8736, 3826'3752, 4031'0784, 4246'7328, 4304'6721, 4534'9632, 4777'5744, 5033'1648, 5101'8336, 5374'7712, 5662'3104, 5739'5628, 6046'6176, 6370'0992, 6710'8864, 6802'4448, 7166'3616, 7549'7472, 7652'7504, 8062'1568, 8493'4656, 8609'3442, 9069'9264, 9555'1488, 1'0066'3296, 1'0203'6672, 1'0749'5424, 1'1324'6208, 1'1479'1256, 1'2093'2352, 1'2740'1984, 1'2914'0163, 1'3421'7728, 1'3604'8896, 1'4332'7232, 1'5099'4944, 1'5305'5008, 1'6124'3136, 1'6986'9312, 1'7218'6884, 1'8139'8528, 1'9110'2976, 2'0132'6592, 2'0407'3344, 2'1499'0848, 2'2649'2416, 2'2958'2512, 2'4186'4704, 2'5480'3968, 2'5828'0326, 2'6843'5456, 2'7209'7792, 2'8665'4464, 3'0198'9888, 3'0611'0016, 3'2248'6272, 3'3973'8624, 3'4437'3768, 3'6279'7056, 3'8220'5952, 3'8742'0489, 4'0265'3184, 4'0814'6688, 4'2998'1696, 4'5298'4832, 4'5916'5024, 4'8372'9408, 5'0960'7936, 5'1656'0652, 5'3687'0912, 5'4419'5584, 5'7330'8928, 6'0397'9776, 6'1222'0032, 6'4497'2544, 6'7947'7248, 6'8874'7536, 7'2559'4112, 7'6441'1904, 7'7484'0978, 8'0530'6368, 8'1629'3376, 8'5996'3392, 9'0596'9664, 9'1833'0048, 9'6745'8816}, Sedgewick[]{1, 5, 19, 41, 109, 209, 505, 929, 2161, 3905, 8929, 1'6001, 3'6289, 6'4769, 14'6305, 26'0609, 58'7521, 104'5505, 235'4689, 418'8161, 942'7969, 1676'4929, 3773'0305, 6708'4289, 1'5095'8081, 2'6838'6305, 6'0390'6049};
        static const Vector<int> increments{Sedgewick};
        int last_increment{increments.qfind(size() / 2) - 1};
        for (int width{increments[last_increment]}, length{hi - lo}, i{0}; 0 <= last_increment; width = increments[--last_increment >= 0 ? last_increment : 0], i = 0)
            for (int remainder{length % width}, matrix{hi - remainder}; i < width; ++i)
                insertion_sort(lo + i, matrix + i + (i < remainder ? width : 0), width);
    }
    template<typename T> void Sequential<T>::selection_sort(const int lo, int hi) const {
        for (int idx_max_elem{lo}; lo < --hi; swap((*this)[idx_max_elem], (*this)[hi]), idx_max_elem = lo)
            for (int i{lo}; i <= hi; ++i)
                (*this)[i] <= (*this)[idx_max_elem] ? 0 : (idx_max_elem = i);
    }
    template<typename T> void Sequential<T>::heap_sort(int lo, int hi) const {
        CHeap<T> heap{};
        while (lo != hi--)
            heap.enque(const_cast<Sequential*>(this)->pick(hi));
        while (heap.size() != 0)
            const_cast<Sequential*>(this)->insert(lo++, heap.deque());
    }

    template<typename T> void Sequential<T>::quick_pivot(int& pivot, int hi) const {
        for (bool move_hi{(--hi, true)}; pivot < hi; move_hi = !move_hi, swap((*this)[pivot], (*this)[hi]))
            if (const auto go_on{[this, &pivot, &hi]() -> bool {
                    return pivot != hi && (*this)[pivot] <= (*this)[hi];
                }};
                move_hi)
                while (go_on())
                    --hi;
            else
                while (go_on())
                    ++pivot;
    }

    template<typename T> T& Sequential<T>::top_k(const int k, const char* which) const {
        switch (npos_off(k, size() / 2), (0 <= k && k < size()) ? 0 : throw "kth element with k out of [0,size)"; option(which, "heap", "quick", "BFPRT")) {
            case 1:
            case 2:
                return const_cast<T&>(static_cast<const T&>(slice<T>{*this}.quick_kth(k, 0, size())));
            case 3:
                [[likely]] default: return BFPRT_kth(k);
        }
    }
    template<typename T> T& Sequential<T>::min() const { return top_k(0); }
    template<typename T> T& Sequential<T>::max() const { return top_k(size() - 1); }

    template<typename T> typename Sequential<T>::iterator Sequential<T>::begin() const { return {this, 0}; }
    template<typename T> typename Sequential<T>::iterator Sequential<T>::end() const { return {this, size()}; }
    template<typename T> int Sequential<T>::hash() const {
        int polynomial_hash_code{0}, x{1};
        for (static const constexpr int radix{37}; const auto& e : *this)
            polynomial_hash_code += hasher<decltype(e)>{}(e)*x, x *= radix;
        return Cont::hash() ^ polynomial_hash_code;
    }

    template<typename T> T& Sequential<T>::quick_kth(const int& k, const int lo, const int hi) const {
        int pivot{lo};
        return (swap((*this)[lo], (*this)[lo + rand(hi - lo)]), quick_pivot(pivot, hi), k == pivot) ? (*this)[pivot] : (k < pivot ? quick_kth(k, lo, pivot) : quick_kth(k, pivot + 1, hi));
    }
    template<typename T> T& Sequential<T>::BFPRT_kth(const int k) const {
        static constexpr const int section_len{5};
        static constexpr const struct {
            const int& section_len;
            const T& operator()(const int k, slice<T> input) const {
                Vector<slice<T>> sections{};
                for (int head{0}, input_size{input.size()}; head < input_size; head += section_len)
                    sections.push(static_cast<slice<T>&&>(slice<T>{input, head, head + ((head + section_len <= input_size) ? section_len : (input_size - head))}.sort("quick")));
                if (sections.size() == 1)
                    return sections[0][k];
                slice<T>& medians{input = {}};
                for (const auto& section : sections)
                    medians.push(section[section.size() / 2]);
                const T& median_of_medians{(*this)(medians.size() / 2, static_cast<slice<T>&&>(medians))};
                slice<T> less_than_median{}, &equal_to_median{medians = {}}, greater_than_median{};
                for (const auto& section : sections)
                    for (const T& e : section)
                        (e == median_of_medians ? equal_to_median : (e < median_of_medians ? less_than_median : greater_than_median)).push(e);
                if (sections = {}; k < less_than_median.size())
                    return (*this)(k, static_cast<slice<T>&&>(less_than_median));
                else if (const int cnt_less{less_than_median.size()}, cnt_equal{equal_to_median.size()}; k < cnt_less + cnt_equal)
                    return median_of_medians;
                else
                    return (*this)(k - (cnt_less + cnt_equal), static_cast<slice<T>&&>(greater_than_median));
            }
        } BFPRT{section_len};
        return const_cast<T&>(BFPRT(k, *this));
    }

    template<typename T> int Sequential<T>::grep(const slice<T>& pattern, const char* which) const { return grep(pattern, 0, npos, which); }
    template<typename T> int Sequential<T>::grep(const slice<T>& pattern, const int lo, int hi, const char* which) const {
        if (const int length{(npos_off(hi), pattern.size())}; !(0 <= lo && lo < hi && hi <= size() && length <= hi - lo)) [[unlikely]]
            return (length == 0 && lo == hi) ? 0 : npos;
        else if (length == 0) [[unlikely]]
            return 0;
        else
            switch (hi -= length - 1; option(which, "RK", "KR", "BM", "brute", "KMP")) {
                case 1:
                case 2:
                    return npos;
                case 3:
                    return npos;
                case 4:
                    return brute_grep(pattern, lo, hi);
                case 5:
                    [[likely]] default: return KMP_grep(pattern, lo, hi);
            }
    }

    template<typename T> int Sequential<T>::brute_grep(const slice<T>& pattern, int lo, const int hi) const {
        for (const int length{pattern.size()}; lo < hi; ++lo)
            if (pattern == slice<T>{*this, lo, lo + length})
                return lo;
        return npos;
    }
    template<typename T> int Sequential<T>::KMP_grep(const slice<T>& pattern, int lo, const int hi) const {
        const auto chcmp{[&pattern](const T& ch, const int& pattern_i) -> bool {
            return pattern_i == -1 || static_cast<const T&>(pattern[pattern_i]) == ch;
        }};
        Vector<int> next_table{};
        next_table.push(-1);
        for (int i{1}, next; i < pattern.size(); ++i)
            for (bool go_on{(next = next_table[i - 1], true)}; go_on; go_on ? (next = next_table[next]) : 0)
                if (chcmp(pattern[i - 1], next) ? (go_on = false, true) : false)
                    next_table.push(pattern[i] != pattern[next + 1] ? (next + 1) : next_table[next + 1]);
        int pattern_i{0};
        for (int& hi_tail{const_cast<int&>(hi) += pattern.size() - 1}; lo < hi_tail && pattern_i < pattern.size();)
            if (chcmp((*this)[lo], pattern_i))
                ++lo, ++pattern_i;
            else
                pattern_i = next_table[pattern_i];
        return pattern_i == pattern.size() ? (lo - pattern_i) : npos;
    }

} // namespace conts
namespace conts {
    /* impl for Cont */

    Cont& Cont::dsize(const int det) noexcept(false) {
        (_size += det) >= 0 ? 0 : throw "size < 0";
        return *this;
    }
    int Cont::option(const char* const which) { return npos; }
    template<typename... cstr> int Cont::option(const char* const which, const char* const first_option, const cstr... rest_options) { return String{which} == String{first_option} ? 1 : 1 + option(which, rest_options...); }
    int Cont::rand(const int mod) {
        static int soil{11'4514};
        return ((soil += std::rand()) & ~npos) % mod;
    }
    int& Cont::npos_off(const int& num, const int target_value) const { return const_cast<int&>(num) = (num != npos ? num : npos_off(target_value, size())); }
    Cont::~Cont() { dsize(-size()); }
    [[nodiscard("information is delivered in form of const char *, but std::ios_base")]] const char* Cont::info(const char* const which) {
        switch (option(which)) {
            [[likely]] default:
                return "class Cont, in namespace conts, by Shynur <one.last.kiss@qq.com> <Shynur@foxmail.com> 2022. ";
        }
    }
    bool Cont::operator<(const Cont& b) const { return size() < b.size(); }
    int Cont::size() const { return _size; }
    int Cont::hash() const { return hasher<decltype(size())>{}(size()); }

    namespace utilities {}
    namespace safely {}
} // namespace conts
namespace std {

    template<typename T> struct hash<::conts::Sequential<T>>: ::conts::hasher<::conts::Sequential<T>> {};
    template<typename T> struct hash<::conts::Vector<T>>: hash<::conts::Sequential<T>> {};
    template<typename T> struct hash<::conts::List<T>>: hash<::conts::Sequential<T>> {};

    template<typename Vertex_data_T, typename Edge_data_T> struct hash<::conts::Graph<Vertex_data_T, Edge_data_T>>: ::conts::hasher<::conts::Graph<Vertex_data_T, Edge_data_T>> {};

    template<> struct hash<::conts::BitMap>: ::conts::hasher<::conts::BitMap> {};

} // namespace std
namespace conts {
    /* impl for some utilities */

    template<typename T1, typename T2> bool operator<(const T1& a, const T2& b) { return b > a; }
    template<typename T1, typename T2> inline bool operator<=(const T1& a, const T2& b) { return !(b < a); }
    template<typename T1, typename T2> inline bool operator>=(const T1& a, const T2& b) { return b <= a; }
    template<typename T1, typename T2> inline bool operator>(const T1& a, const T2& b) { return !(b >= a); }
    template<typename T1, typename T2> bool operator==(const T1& a, const T2& b) { return a <= b && a >= b; }
    template<typename T1, typename T2> inline bool operator!=(const T1& a, const T2& b) { return !(a == b); }
    template<typename T1, typename T2> inline T1 operator+(T1 a, T2 b) { return a += static_cast<T2&&>(b); }

} // namespace conts
#include <string>
#include <iostream>
namespace conts::utilities {
    template<typename cont_T> constexpr bool is_Cont() { return std::is_base_of<Cont, cont_T>::value; }
    template<typename cont_T> using val_t = std::remove_cvref_t<decltype(*(cont_T{}.begin()))>;

    inline std::istream& operator>>(std::istream& in, String& s) {
        std::string std_str{};
        in >> std_str;
        s = std_str.c_str();
        return in;
    }
    inline std::istream& operator>>(std::istream& in, uString& us) {
        String s{};
        in >> s;
        us = static_cast<const char*>(s);
        return in;
    }
    inline String as_String(const std::string& std_s) { return std_s.c_str(); }
    inline uString as_uString(const std::string& std_s) { return static_cast<const char*>(as_String(std_s)); }

    String convert(unsigned num, const unsigned char base = 16) {
        static constexpr const char symbols[]{
            '0',
            '1',
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',
            'A',
            'B',
            'C',
            'D',
            'E',
            'F',
            'G',
            'H',
            'I',
            'J',
            'K',
            'L',
            'M',
            'N',
            'O',
            'P',
            'Q',
            'R',
            'S',
            'T',
            'U',
            'V',
            'W',
            'X',
            'Y',
            'Z',
        };
        base <= sizeof(symbols) / sizeof(symbols[0]) ? 0 : (std::cerr << "base converted to must be in [2,36]\n", 0);
        String number_sequen{};
        Vector<unsigned char> inv_number_sequen{};
        do {
            inv_number_sequen.push(num % base), num /= base;
        } while (num != 0);
        while (inv_number_sequen.size() != 0)
            number_sequen.push(symbols[inv_number_sequen.pop()]);
        return number_sequen;
    }
    unsigned convert(const String& number_sequen, const unsigned char base = 10) {
        unsigned num{0};
        for (unsigned hi{0}; hi < number_sequen.size(); ++hi)
            (num *= base) += number_sequen[hi] - (number_sequen[hi] <= '9' ? '0' : ((number_sequen[hi] <= 'Z' ? 'A' : 'a') - 10));
        return num;
    }

    Vector<unsigned> eight_queens(const unsigned n = 8) {
        n >= 4 ? 0 : (std::cerr << "n < 4 in eight(N)_queens-problem\n", 0);
        struct Queen final {
            const unsigned &n, x, y;
            bool operator==(const Queen& b) const { return x == b.x || y == b.y || x + y == b.x + b.y || x + b.y == b.x + y; }
            Queen(const unsigned x, const unsigned y, const unsigned& n): n{n}, x{x}, y{y} {}
        };
        Vector<Queen> queens{};
        queens.push({1, 1, n});
        for (unsigned x{1}, y{2}; y <= n; x <= n ? (++y, x = 1) : (--y, x = queens.pop().x + 1))
            for (; x <= n; ++x)
                if (Queen queen{x, y, n}; queens.find(queen) < 0) {
                    queens.push(queen);
                    break;
                }
        Vector<unsigned> ans{};
        while (queens.size() != 0)
            ans.push(queens.pop().x);
        return ans;
    }

    bool prime(const unsigned number) {
        const unsigned cnt{static_cast<unsigned>(BitMap{static_cast<int>(number)}.highest()) / 2 + 1};
        if (BitMap _{}; number < 2)
            return false;
        else {
            for (unsigned i{0}; i < cnt; ++i)
                _[i] = true;
            const_cast<unsigned&>(cnt) = _.as_int();
        }
        BitMap sifter{};
        for (unsigned i{2}; i <= cnt; ++i)
            sifter[i] = true;
        for (unsigned i{2}; i <= cnt; ++i)
            if (sifter[i])
                if (number % i == 0)
                    return false;
                else
                    for (unsigned j{i * 2}; j <= cnt; j += i)
                        sifter[j] = false;
        return true;
    }
} // namespace conts::utilities
namespace conts::safely {
    using utilities::is_Cont;
    using utilities::val_t;

    using utilities::operator>>;
    using utilities::as_String;
    using utilities::as_uString;
    using utilities::convert;
    using utilities::eight_queens;
    using utilities::prime;

    using conts::Vector;
    template<typename T> using slice [[deprecated("slice which is only be impl for slice-semantic cannot perform most algorithms")]] = conts::slice<T>;
    using conts::String;
    using conts::uString;
    using conts::BitMap;
    using conts::List;
    using conts::Graph;
    using conts::Hash;
    using conts::CHeap;

    constexpr auto npos = Cont::npos;
    constexpr auto VISITED = Graph<char, char>::VISITED;
    constexpr auto DISCOVERED = Graph<char, char>::DISCOVERED;
    constexpr auto UNDISCOVERED = Graph<char, char>::UNDISCOVERED;
    constexpr auto TREE = Graph<char, char>::TREE;
    constexpr auto CROSS = Graph<char, char>::CROSS;
    constexpr auto FORWARD = Graph<char, char>::FORWARD;
    constexpr auto BACKWARD = Graph<char, char>::BACKWARD;
    constexpr auto UNDETERMINED = Graph<char, char>::UNDETERMINED;
} // namespace conts::safely
using namespace conts::safely;
