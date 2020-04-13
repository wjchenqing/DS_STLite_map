/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	typedef pair<const Key, T> value_type;
	class RBT{
    public:
	    struct node{
	        bool color;
	        value_type *val;
	        node *son[2];
	        node *par;
	        node(): color(false), val(nullptr), son{nullptr, nullptr}, par(nullptr){}
	        node(const node &other){
	            color = other.color;
	            val = new value_type(other.val);
	            son = {other.son[0], other.son[1]};
	            par = other.par;
	        }
	        explicit node(const value_type &v, node *n = nullptr){
	            val = new value_type(v);
	            son[0] = son[1] = n;
	        }
	        ~node(){
	            if (val != nullptr) delete val;
	        }
	        node &operator=(const node &other){
	            if(this == &other)
                    return *this;
	            delete val;
                color = other.color;
                val = new value_type(*other.val);
                son[0] = other.son[0];
                son[1] = other.son[1];
                par = other.par;
                return *this;
	        }
	    };
	    node *nil, *root;
	    size_t siz;
	    Compare cmp;
	    void build(node *&tmp, const node *o_tmp, node *p, const node *o_n){
            if(o_tmp == o_n){
                tmp = nil;
                return;
            }
            tmp = new node(*o_tmp->val, nil);
            tmp->color = o_tmp->color;
            tmp->par = p;
            p->son[o_tmp == o_tmp->par->son[1]] = tmp;
            build(tmp->son[0], o_tmp->son[0], tmp, o_n);
            build(tmp->son[1], o_tmp->son[1], tmp, o_n);
	    }
	    RBT(){
	        nil = new node;
	        nil->par = nil->son[0] = nil->son[1] = nil;
	        root = nil;
            siz = 0;
	    }
	    RBT(const RBT &other){
	        nil = new node;
	        nil->par = nil->son[0] = nil->son[1] = nil;
            siz = other.siz;
	        build(root, other.root, nil, other.nil);
	    }
	    ~RBT(){
	        if(root != nil)
	            clear(root);
	        delete nil;
	    }
	    void clear(node *tmp){
	        if(tmp->son[0] != nil)
	            clear(tmp->son[0]);
	        if(tmp->son[1] != nil)
	            clear(tmp->son[1]);
	        delete tmp;
	    }
	    RBT &operator= (const RBT &other){
	        if(this == &other)
                return *this;
	        if(root != nil)
	            clear(root);
	        root = nullptr;
            nil->par = nil->son[0] = nil->son[1] = nil;
            siz = other.siz;
            build(root, other.root, nil, other.nil);
            return *this;
	    }
	    node* search(const Key &k) const {
	        if (root == nil)
                return nil;
	        node *tmp = root;
	        while (tmp != nil && (cmp(tmp->val->first, k) || cmp(k, tmp->val->first)))
	            tmp = cmp(k, tmp->val->first) ? tmp->son[0] : tmp->son[1];
            return (tmp == nil) ? nil : tmp;
	    }
	    node* search_insert(const Key &k) {
	        node *tmp = root;
	        while(true){
	            if(cmp(k, tmp->val->first)){
	                if(tmp->son[0] == nil)
                        return tmp;
	                tmp = tmp->son[0];
                    continue;
	            } else if(cmp(tmp->val->first, k)){
	                if(tmp->son[1] == nil)
                        return tmp;
	                tmp = tmp->son[1];
                    continue;
	            }else return nil;
	        }
	    }
	    node *begin(){
	        if(siz == 0)
                return nil;
	        node *tmp = root;
	        while (tmp->son[0] != nil)
	            tmp = tmp->son[0];
            return tmp;
	    }
	    node *end(){
	        return nil;
	    }
	    const node *cbegin() const {
            if(siz == 0)
                return nil;
            node *tmp = root;
            while (tmp->son[0] != nil)
                tmp = tmp->son[0];
            return tmp;
	    }
	    const node *cend() const {
            return nil;
	    }
	    node *pre_end() const {
            if(siz == 0)
                return nil;
            node *tmp = root;
            while (tmp->son[1] != nil)
                tmp = tmp->son[1];
            return tmp;
	    }
	    node *last(const node *cur) const {
	        if (cur == nil)
                return pre_end();
	        if(cur->son[0] != nil){
	            node *tmp = cur->son[0];
	            while (tmp->son[1] != nil)
	                tmp = tmp->son[1];
                return tmp;
	        } else if(cur->par != nil && cur == cur->par->son[1])
                return cur->par;
	        else{
	            node *tmp = cur->par;
	            while (tmp->par != nil && tmp == tmp->par->son[0]) {
	                tmp = tmp->par;
	            }
	            if (tmp->par != nil)
                    return tmp->par;
                else
                    return nil;
	        }
	    }
	    node* nex(const node *cur) const {
            if(cur->son[1] != nil){
                node *tmp = cur->son[1];
                while (tmp->son[0] != nil)
                    tmp = tmp->son[0];
                return tmp;
            } else if(cur->par != nil && cur == cur->par->son[0])
                return cur->par;
            else {
                node *tmp = cur->par;
                while (tmp->par != nil && tmp == tmp->par->son[1]){
                    tmp = tmp->par;
                }
                if (tmp->par != nil)
                    return tmp->par;
                else
                    return nil;
            }
	    }
	    void rotate(node *s){ // son of the edge
	        node *p = s->par, *pp = p->par;
	        int r = s == p->son[1], l = r ^ 1;
	        if(p == root)
	            root = s;
	        else {
                pp->son[p == pp->son[1]] = s;
//                s->par = pp;
            }
            s->par = pp;
	        p->par = s;
	        p->son[r] = s->son[l];
	        if(s->son[l] != nil)
	            s->son[l]->par = p;
	        s->son[l] = p;
	    }
	    void fix_insert(node *z){
	        while (z->color == 1 && z->par->color == 1){
	            int r = z->par == z->par->par->son[0], l = r ^ 1;
	            node *y = z->par->par->son[r];
	            if(y->color == 1){
	                z->par->color = 0;
	                y->color = 0;
	                z->par->par->color = 1;
	                z = z->par->par;
	            } else {
                    if (z == z->par->son[r]) {
                        z = z->par;
                        rotate(z->son[r]);
                    }
                    z->par->color = 0;
                    z->par->par->color = 1;
                    rotate(z->par->par->son[l]);
                }
	        }
	        root->color = 0;
	    }
	    node *insert(const value_type &v){
	        if(nil == root){
	            ++siz;
	            root = new node(v,nil);
	            root->color = 0;
	            root->par = nil;
                return root;
	        }
	        node *pre = search_insert(v.first);
	        if(pre == nil)
                return nil;
	        ++siz;
	        node *cur = new node(v, nil);
	        cur->color = 1;
	        cur->par = pre;
	        pre->son[cmp(pre->val->first, v.first)] = cur;
	        fix_insert(cur);
	        nil->color = 0;
	        nil->par = nil->son[0] = nil->son[1] = nil;
            return cur;
	    }
	    void fix_earse(node *x){
	        while (x != root && x->color == 0){
	            int r = x == x->par->son[0], l = r ^ 1;
	            node *w = x->par->son[r];
	            if (w->color == 1){
	                w->color = 0;
	                x->par->color = 1;
                    rotate(w);
	                w = x->par->son[r];
	            }
	            if (w->son[l]->color == 0 && w->son[r]->color == 0){
	                w->color = 1;
	                x = x->par;
	            } else {
                    if (w->son[r]->color == 0) {
                        w->son[l]->color = 0;
                        w->color = 1;
                        rotate(w->son[l]);
                        w = x->par->son[r];
                    }
                    w->color = x->par->color;
                    x->par->color = 0;
                    w->son[r]->color = 0;
                    rotate(x->par->son[r]);
                    x = root;
                }
	        }
	        x->color = 0;
	    }
	    void erase(node *z){
	        --siz;
	        node *y;
	        node *x;
	        if (z->son[0] == nil || z->son[1] == nil){
	            y = z;
            } else {
                y = nex(z);
            }
            x = y->son[y->son[0] == nil];
            x->par = y->par;
            if (y != root)
                y->par->son[y == y->par->son[1]] = x;
            else
                root = x;
            if(y != z) { // exchange y to z;
                if (z != root)
                    z->par->son[z == z->par->son[1]] = y;
                else
                    root = y;
                y->par = z->par;
                z->son[0]->par = y;
                y->son[0] = z->son[0];
                z->son[1]->par = y;
                y->son[1] = z->son[1];
                bool tmp = z->color;
                z->color = y->color;
                y->color = tmp;
                y = z;
            }
            if(y->color == 0)
                fix_earse(x);
            delete y;
            nil->par = nil->son[0] = nil->son[1] = nil;
        }
	}rbt;

	class const_iterator;
	class iterator {
    public:
	    friend const_iterator;
	    typedef pair<const Key, T> value_type;
    public:
		map *curmap;
		typename RBT::node *p;
	public:
		iterator(): curmap(nullptr), p(nullptr){}
		iterator(const iterator &other): curmap(other.curmap), p(other.p){}
		iterator(map *m, typename RBT::node *p): curmap(m), p(p){}

		iterator operator++(int) {
		    if (*this == curmap->end())
		        throw invalid_iterator();
		    typename RBT::node *tmp = p;
		    p = curmap->rbt.nex(tmp);
            return iterator(curmap, tmp);
		}
		iterator & operator++() {
		    if (*this == curmap->end())
		        throw invalid_iterator();
            p = curmap->rbt.nex(p);
            return *this;
		}
		iterator operator--(int) {
		    if (*this == curmap->begin())
		        throw invalid_iterator();
		    typename RBT::node *tmp = p;
		    p = curmap->rbt.last(tmp);
            return iterator(curmap, tmp);
		}
		iterator & operator--() {
		    if (*this == curmap->begin())
		        throw invalid_iterator();
            p = curmap->rbt.last(p);
            return *this;
		}
		value_type & operator*() const {
            return *(p->val);
		}
		bool operator==(const iterator &rhs) const {
            return curmap == rhs.curmap && p == rhs.p;
		}
		bool operator==(const const_iterator &rhs) const {
            return curmap == rhs.curmap && p == rhs.p;
        }

		bool operator!=(const iterator &rhs) const {
            return curmap != rhs.curmap || p != rhs.p;
        }
		bool operator!=(const const_iterator &rhs) const {
            return curmap != rhs.curmap || p != rhs.p;
        }
		value_type* operator->() const noexcept {
            return &*(p->val);
		}
	};
	class const_iterator {
        friend iterator;
        typedef pair<const Key, T> value_type;

    private:
        const map *curmap;
        const typename RBT::node *p;
    public:
        const_iterator() : curmap(nullptr), p(nullptr) {}
        const_iterator(const const_iterator &other) : curmap(other.curmap), p(other.p) {}
        const_iterator(const iterator &other) : curmap(other.curmap), p(other.p) {}
        const_iterator(const map *m, const typename RBT::node *pos) : curmap(m), p(pos) {}

        const_iterator operator++(int) {
            if (*this == curmap->cend())
                throw invalid_iterator();
            const typename RBT::node *tmp = p;
            p = curmap->rbt.nex(tmp);
            return const_iterator(curmap, tmp);
        }
        const_iterator &operator++() {
            if (*this == curmap->cend())
                throw invalid_iterator();
            p = curmap->rbt.nex(p);
            return *this;
        }
        const_iterator operator--(int) {
            if (*this == curmap->cbegin())
                throw invalid_iterator();
            const typename RBT::node *tmp = p;
            p = curmap->rbt.last(tmp);
            return const_iterator(curmap, tmp);
        }
        const_iterator &operator--() {
            if (*this == curmap->cbegin())
                throw invalid_iterator();
            p = curmap->rbt.last(p);
            return *this;
        }

        const value_type &operator*() const {
            return *(p->val);
        }
        bool operator==(const iterator &rhs) const {
            return curmap == rhs.curmap && p == rhs.p;
        }
        bool operator==(const const_iterator &rhs) const {
            return curmap == rhs.curmap && p == rhs.p;
        }
        bool operator!=(const iterator &rhs) const {
            return curmap != rhs.curmap || p != rhs.p;
        }
        bool operator!=(const const_iterator &rhs) const {
            return curmap != rhs.curmap || p != rhs.p;
        }
        const value_type *operator->() const noexcept {
            return &*(p->val);
        }
    };
	map() {}
	map(const map &other) : rbt(other.rbt) {}
	map & operator=(const map &other) = default;
	~map() {}
	T & at(const Key &key) {
        typename RBT::node *tmp = rbt.search(key);
        if (tmp == rbt.nil)
            throw index_out_of_bound();
        else
            return tmp->val->second;
	}
	const T & at(const Key &key) const {
        const typename RBT::node *tmp = rbt.search(key);
        if (tmp == rbt.nil)
            throw index_out_of_bound();
        else
            return tmp->val->second;
	}
	T & operator[](const Key &key) {
        typename RBT::node *tmp = rbt.search(key);
        if (tmp == rbt.nil) {
            typename RBT::node *p = rbt.insert(value_type(key, T()));
            return p->val->second;
        }
        return tmp->val->second;
    }
	const T & operator[](const Key &key) const {
        typename RBT::node *tmp = rbt.search(key);
        if (tmp == rbt.nil)
            throw index_out_of_bound();
        else
            return tmp->val->second;
	}
	iterator begin() {
        return iterator(this, rbt.begin());
	}
	const_iterator cbegin() const {
        return const_iterator(this, rbt.cbegin());
	}
	iterator end() {
        return iterator(this, rbt.end());
	}
	const_iterator cend() const {
        return const_iterator(this, rbt.cend());
	}
	bool empty() const {
        return rbt.siz == 0;
	}
	size_t size() const {
        return rbt.siz;
	}
	void clear() {
	    if (rbt.nil != rbt.root)
	        rbt.clear(rbt.root);
	    rbt.siz = 0;
	    rbt.root = rbt.nil;
	}
	pair<iterator, bool> insert(const value_type &value) {
	    typename RBT::node *tmp = rbt.insert(value);
	    if (tmp == rbt.nil)
            return pair<iterator, bool>(iterator(this, rbt.search(value.first)), 0);
        return pair<iterator, bool>(iterator(this, tmp), 1);
	}
	void erase(iterator pos) {
	    if (pos.curmap != this || pos.p == rbt.end())
	        throw invalid_iterator();
        rbt.erase(pos.p);
	}
	size_t count(const Key &key) const {
	    if (rbt.search(key) == rbt.nil)
            return 0;
        return 1;
	}
	iterator find(const Key &key) {
	    typename RBT::node *tmp = rbt.search(key);
	    if (tmp == rbt.nil)
            return iterator(this, rbt.end());
        return iterator(this, tmp);
	}
	const_iterator find(const Key &key) const {
        typename RBT::node *tmp = rbt.search(key);
        if (tmp == rbt.nil)
            return const_iterator(this, rbt.cend());
        return const_iterator(this, tmp);
	}
};

}

#endif
