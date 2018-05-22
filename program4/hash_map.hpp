// Submitter: huyanhh(Hoang, Huyanh)
// Partner  : monicadc(Cruz, Monica)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming

#ifndef HASH_MAP_HPP_
#define HASH_MAP_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"

using std::cout;
using std::endl;

namespace ics {


#ifndef undefinedhashdefined
#define undefinedhashdefined
    template<class T>
    int undefinedhash (const T& a) {return 0;}
#endif /* undefinedhashdefined */

//Instantiate the templated class supplying thash(a): produces a hash value for a.
//If thash is defaulted to undefinedhash in the template, then a constructor must supply chash.
//If both thash and chash are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedhash value supplied by thash/chash is stored in the instance variable hash.
    template<class KEY,class T, int (*thash)(const KEY& a) = undefinedhash<KEY>> class HashMap {
    public:
        typedef ics::pair<KEY,T>   Entry;
        typedef int (*hashfunc) (const KEY& a);

        //Destructor/Constructors
        ~HashMap ();

        HashMap          (double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);
        explicit HashMap (int initial_bins, double the_load_threshold = 1.0, int (*chash)(const KEY& k) = undefinedhash<KEY>);
        HashMap          (const HashMap<KEY,T,thash>& to_copy, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);
        explicit HashMap (const std::initializer_list<Entry>& il, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template <class Iterable>
        explicit HashMap (const Iterable& i, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);


        //Queries
        bool empty      () const;
        int  size       () const;
        bool has_key    (const KEY& key) const;
        bool has_value  (const T& value) const;
        std::string str () const; //supplies useful debugging information; contrast to operator <<


        //Commands
        T    put   (const KEY& key, const T& value);
        T    erase (const KEY& key);
        void clear ();

        //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
        template <class Iterable>
        int put_all(const Iterable& i);


        //Operators

        T&       operator [] (const KEY&);
        const T& operator [] (const KEY&) const;
        HashMap<KEY,T,thash>& operator = (const HashMap<KEY,T,thash>& rhs);
        bool operator == (const HashMap<KEY,T,thash>& rhs) const;
        bool operator != (const HashMap<KEY,T,thash>& rhs) const;

        template<class KEY2,class T2, int (*hash2)(const KEY2& a)>
        friend std::ostream& operator << (std::ostream& outs, const HashMap<KEY2,T2,hash2>& m);



    private:
        class LN;

    public:
        class Iterator {
        public:
            typedef pair<int,LN*> Cursor;

            //Private constructor called in begin/end, which are friends of HashMap<T>
            ~Iterator();
            Entry       erase();
            std::string str  () const;
            HashMap<KEY,T,thash>::Iterator& operator ++ ();
            HashMap<KEY,T,thash>::Iterator  operator ++ (int);
            bool operator == (const HashMap<KEY,T,thash>::Iterator& rhs) const;
            bool operator != (const HashMap<KEY,T,thash>::Iterator& rhs) const;
            Entry& operator *  () const;
            Entry* operator -> () const;
            friend std::ostream& operator << (std::ostream& outs, const HashMap<KEY,T,thash>::Iterator& i) {
                outs << i.str(); //Use the same meaning as the debugging .str() method
                return outs;
            }
            friend Iterator HashMap<KEY,T,thash>::begin () const;
            friend Iterator HashMap<KEY,T,thash>::end   () const;

        private:
            //If can_erase is false, current indexes the "next" value (must ++ to reach it)
            Cursor                current; //Bin Index and Cursor; stops if LN* == nullptr
            HashMap<KEY,T,thash>* ref_map;
            int                   expected_mod_count;
            bool                  can_erase = true;

            //Helper methods
            void advance_cursors();

            //Called in friends begin/end
            Iterator(HashMap<KEY,T,thash>* iterate_over, bool from_begin);
        };


        Iterator begin () const;
        Iterator end   () const;


    private:
        class LN {
        public:
            LN ()                         : next(nullptr){}
            LN (const LN& ln)             : value(ln.value), next(ln.next){}
            LN (Entry v, LN* n = nullptr) : value(v), next(n){}

            Entry value;
            LN*   next;
        };

        int (*hash)(const KEY& k);  //Hashing function used (from template or constructor)
        LN** map      = nullptr;    //Pointer to array of pointers: each bin stores a list with a trailer node
        double load_threshold;      //used/bins <= load_threshold
        int bins      = 1;          //# bins in array (should start >= 1 so hash_compress doesn't % 0)
        int used      = 0;          //Cache for number of key->value pairs in the hash table
        int mod_count = 0;          //For sensing concurrent modification


        //Helper methods
        int   hash_compress        (const KEY& key)          const;  //hash function ranged to [0,bins-1]
        LN*   find_key             (const KEY& key) const;           //Returns reference to key's node or nullptr
        LN*   copy_list            (LN*   l)                 const;  //Copy the keys/values in a bin (order irrelevant)
        LN**  copy_hash_table      (LN** ht, int bins)       const;  //Copy the bins/keys/values in ht tree (order in bins irrelevant)

        void  ensure_load_threshold(int new_used);                   //Reallocate if load_factor > load_threshold
        void  delete_hash_table    (LN**& ht, int bins);             //Deallocate all LN in ht (and the ht itself; ht == nullptr)
    };





////////////////////////////////////////////////////////////////////////////////
//
//HashMap class and related definitions

//Destructor/Constructors

    template<class KEY,class T, int (*thash)(const KEY& a)>
    HashMap<KEY,T,thash>::~HashMap() {
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    HashMap<KEY,T,thash>::HashMap(double the_load_threshold, int (*chash)(const KEY& k))
            :hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash){
        if (hash == (hashfunc) undefinedhash<KEY>)
            throw TemplateFunctionError("HashMap::default constructor: neither specified");
        if (thash != chash && thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY>)
            throw TemplateFunctionError("HashMap::default constructor: both specified and different");

        load_threshold = the_load_threshold;
        map = new LN*[bins];
        map[0] = new LN();
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    HashMap<KEY,T,thash>::HashMap(int initial_bins, double the_load_threshold, int (*chash)(const KEY& k))
            :hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash) {
        if (hash == (hashfunc) undefinedhash<KEY>)
            throw TemplateFunctionError("BSTMap::default constructor: neither specified");
        if (thash != chash && thash!= (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY>)
            throw TemplateFunctionError("BSTMap::default constructor: both specified and different");

        load_threshold = the_load_threshold;
        bins = initial_bins;
        map = new LN*[bins];
        for (int i = 0; i < bins; i++)
            map[i] = new LN();
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    HashMap<KEY,T,thash>::HashMap(const HashMap<KEY,T,thash>& to_copy, double the_load_threshold, int (*chash)(const KEY& a))
            :hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash) {
        if (hash == (hashfunc) undefinedhash<KEY>)
            hash = chash;
        if (thash != (hashfunc)undefinedhash<KEY> && chash != (hashfunc)undefinedhash<KEY> && thash != chash)
            throw TemplateFunctionError("HashMap::default constructor: both specified and different");

        bins = to_copy.bins;
        load_threshold = the_load_threshold;

        map = new LN*[bins];
        for (int i = 0; i < bins; i++)
            map[i] = new LN();

        if(hash == chash) {
            map = copy_hash_table(to_copy.map, to_copy.bins);
            used = to_copy.used;
        }
        else  { //put_all
            used = put_all(to_copy);
            mod_count = 0;
        }
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    HashMap<KEY,T,thash>::HashMap(const std::initializer_list<Entry>& il, double the_load_threshold, int (*chash)(const KEY& k))
      :hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash){

        if (hash == (hashfunc) undefinedhash<KEY>)
            throw TemplateFunctionError("BSTMap::default constructor: neither specified");


        map = new LN*[bins];
        map[0] = new LN();
        load_threshold = the_load_threshold;
        for (const Entry& m_entry : il)
            put(m_entry.first,m_entry.second);
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    template <class Iterable>
    HashMap<KEY,T,thash>::HashMap(const Iterable& i, double the_load_threshold, int (*chash)(const KEY& k))
            :hash(thash != (hashfunc)undefinedhash<KEY> ? thash : chash){
        if (hash == (hashfunc) undefinedhash<KEY>)
            throw TemplateFunctionError("BSTMap::default constructor: neither specified");

        map = new LN*[bins];
        map[0] = new LN(); //
        load_threshold = the_load_threshold;
        for (Entry m_entry : i)
            put(m_entry.first,m_entry.second);
    }


////////////////////////////////////////////////////////////////////////////////
//
//Queries

    template<class KEY,class T, int (*thash)(const KEY& a)>
    bool HashMap<KEY,T,thash>::empty() const {
        return used == 0;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    int HashMap<KEY,T,thash>::size() const {
        return used;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    bool HashMap<KEY,T,thash>::has_key (const KEY& key) const {
        for (int i = 0; i < bins; i++)
            for (LN* p = map[i]; p != nullptr; p = p->next)
                if (p->value.first == key)
                    return true;
        return false;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    bool HashMap<KEY,T,thash>::has_value (const T& value) const {
        for (int i = 0; i < bins; i++)
            for (LN* p = map[i]; p != nullptr; p = p->next)
                if (p->value.second == value)
                    return true;
        return false;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    std::string HashMap<KEY,T,thash>::str() const {
        std::ostringstream answer;
        for(int i = 0; i < bins; i++)
            for (LN *p = map[i]; p != nullptr; p = p->next) {
                if (p->next != nullptr)
                    answer << p->value.first << "->" << p->value.second;
            }
        return answer.str();
    }


////////////////////////////////////////////////////////////////////////////////
//
//Commands

    template<class KEY,class T, int (*thash)(const KEY& a)>
    T HashMap<KEY,T,thash>::put(const KEY& key, const T& value) {
        if(!has_key(key))
            ensure_load_threshold(++used);
        mod_count++;
        for (LN* p = map[hash_compress(key)]; p != nullptr; p = p->next)
            if (p->value.first == key && p->next != nullptr) {
                T old_value = p->value.second;
                p->value.second = value;
                cout << "old val" << old_value << endl;
                cout << "val" << value << endl;
                return (old_value == T() ? value : old_value);
            } else if (p->next == nullptr)
            {
                map[hash_compress(key)] = new LN(Entry(key, value), map[hash_compress(key)]);
            }
        return map[hash_compress(key)]->value.second;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    T HashMap<KEY,T,thash>::erase(const KEY& key) {
        if (find_key(key) == nullptr) {
            std::ostringstream answer;
            answer << "HashMap::erase: key(" << key << ") not in Map";
            throw KeyError(answer.str());
        }

        LN *p = find_key(key);
        LN* to_delete = p->next;
        T to_return = p->value.second;
        p->value = to_delete->value;
        p->next = to_delete->next;
        ++mod_count;
        used--;
        delete to_delete;
        return to_return;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    void HashMap<KEY,T,thash>::clear() {
//        bins = 1;
//        map = new LN*[bins];
//        map[0] = new LN();
//        used = 0;
//        mod_count++;

        for (int i = 0; i < bins; i++)
        {
            for (LN *p = map[i]; p != nullptr;) {
                if (p->next == nullptr)
                {
                    map[i] = p;
                    p = p->next;
                }
                else
                {
                    LN* to_delete = p;
                    p = p->next;
                    delete to_delete;
                }
            }
        }

        used = 0;
        mod_count++;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    template<class Iterable>
    int HashMap<KEY,T,thash>::put_all(const Iterable& i) {
        int count = 0;
        for(auto &entry: i) {
            count++;
            put(entry.first, entry.second);
        }
        return count;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Operators

    template<class KEY,class T, int (*thash)(const KEY& a)>
    T& HashMap<KEY,T,thash>::operator [] (const KEY& key) {
        LN* monica = find_key(key); //pointer to K,V pair
        if (monica != nullptr)
            return monica->value.second;
        put(key, T());
        return find_key(key)->value.second;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    const T& HashMap<KEY,T,thash>::operator [] (const KEY& key) const {
        LN* monica = find_key(key);
        if (monica != nullptr)
            return monica->value.second;
        std::ostringstream answer;
        answer << "HashMap::operator []: key(" << key << ") not in Map";
        throw KeyError(answer.str());
    }

    template<class KEY,class T, int (*thash)(const KEY& a)>
    HashMap<KEY,T,thash>& HashMap<KEY,T,thash>::operator = (const HashMap<KEY,T,thash>& rhs) {
        if (this == &rhs)
            return *this;

        load_threshold = rhs.load_threshold;

        map = new LN*[bins];
        for (int i = 0; i < bins; i++)
            map[i] = new LN();

        bins = rhs.bins;

        if(hash != rhs.hash)
            hash = rhs.hash;
        used = rhs.used;
        map = copy_hash_table(rhs.map, rhs.bins);

        mod_count++;
        return *this;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    bool HashMap<KEY,T,thash>::operator == (const HashMap<KEY,T,thash>& rhs) const {
        if (this == &rhs)
            return true;

        if (used != rhs.size())
            return false;


        for (int i=0; i<bins; ++i) {
            for (LN* p = map[i]; p != nullptr; p = p->next)
            {
                if(p->next != nullptr)
                    if (rhs[p->value.first] != p->value.second)
                        return false;
            }
        }
        return true;

    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    bool HashMap<KEY,T,thash>::operator != (const HashMap<KEY,T,thash>& rhs) const {
        return !(*this == rhs);
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    std::ostream& operator << (std::ostream& outs, const HashMap<KEY,T,thash>& m) {
        outs << "map[";
       outs << m.str();
        outs << "]";
        return outs;
    }


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

    template<class KEY,class T, int (*thash)(const KEY& a)>
    auto HashMap<KEY,T,thash>::begin () const -> HashMap<KEY,T,thash>::Iterator {
        return Iterator(const_cast<HashMap<KEY,T,thash>*>(this), true);
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    auto HashMap<KEY,T,thash>::end () const -> HashMap<KEY,T,thash>::Iterator {
        return Iterator(const_cast<HashMap<KEY,T,thash>*>(this), false);
    }


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

    template<class KEY,class T, int (*thash)(const KEY& a)>
    int HashMap<KEY,T,thash>::hash_compress (const KEY& key) const {
        return abs(hash(key)) % bins;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    typename HashMap<KEY,T,thash>::LN* HashMap<KEY,T,thash>::find_key (const KEY& key) const {
        for (int i = 0;i < bins; i++)
            for (LN* p = map[i];p != nullptr; p = p->next)
                if (p->value.first == key)
                    return p;
        return nullptr;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    typename HashMap<KEY,T,thash>::LN* HashMap<KEY,T,thash>::copy_list (LN* l) const {
        LN* to_return = new LN();
        LN* lhs = to_return;
        for (LN* p = l; p != nullptr; p = p->next, lhs = lhs->next) {
            if(p->next == nullptr)
                lhs = new LN();
            else {
                lhs->value = p->value;
                lhs->next = p->next;
            }
        }
        return to_return;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    typename HashMap<KEY,T,thash>::LN** HashMap<KEY,T,thash>::copy_hash_table (LN** ht, int bins) const {
        LN** return_map = new LN*[bins];
        for(int i = 0; i < bins; i++)
            return_map[i] = copy_list(ht[i]);

        return return_map;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    void HashMap<KEY,T,thash>::ensure_load_threshold(int new_used) {
        if (new_used / bins <= load_threshold)
            return;
        LN** old_map = map;
        bins *= 2;
        map = new LN*[bins];
        for (int i = 0; i < bins; i++)
            map[i] = new LN();
        for (int i = 0; i < bins/2; i++) {
            for (LN *p = old_map[i]; p != nullptr; p = p->next) {
                if (p->next != nullptr)
                    map[hash_compress(p->value.first)] = new LN(Entry(p->value.first, p->value.second), map[hash_compress(p->value.first)]);
            }
        }
        delete old_map;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    void HashMap<KEY,T,thash>::delete_hash_table (LN**& ht, int bins) {
    }






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

    template<class KEY,class T, int (*thash)(const KEY& a)>
    void HashMap<KEY,T,thash>::Iterator::advance_cursors(){
            if(current.second->next->next != nullptr) //not a trailer node
                current.second = current.second->next;
            else {
                for(int i = current.first + 1; i < ref_map->bins; i++){
                    for(LN * p = ref_map->map[i]; p!= nullptr; p = p->next)
                    {
                        if(p->next != nullptr) {
                            current.first = i;
                            current.second = p;
                            return;
                        }
                    }

                }
                current.first = -1;
                current.second = nullptr;
            }
        }



    template<class KEY,class T, int (*thash)(const KEY& a)>
    HashMap<KEY,T,thash>::Iterator::Iterator(HashMap<KEY,T,thash>* iterate_over, bool from_begin)
            : ref_map(iterate_over) {
        expected_mod_count = ref_map->mod_count;

        if(!from_begin || ref_map->empty())
        {
            current.first = -1;
            current.second = nullptr;
        }
        else
        {
            for(int i = 0; i < ref_map->bins; i++)
            {
                for(LN* p = ref_map->map[i]; p != nullptr; p = p->next)
                    if(p->next != nullptr) {
                        current.first = i;
                        current.second = p;
                        goto theEnd; //DANGER
                    }
            }
            theEnd:;
        }
    }

    template<class KEY,class T, int (*thash)(const KEY& a)>
    HashMap<KEY,T,thash>::Iterator::~Iterator()
    {}


    template<class KEY,class T, int (*thash)(const KEY& a)>
    auto HashMap<KEY,T,thash>::Iterator::erase() -> Entry {
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("HashMap::Iterator::erase");
        if (!can_erase)
            throw CannotEraseError("HashMap::Iterator::erase Iterator cursor already erased");
        if (current.second == nullptr)
            throw CannotEraseError("HashMap::Iterator::erase Iterator cursor beyond data structure");

        can_erase = false;
        Entry to_return = current.second->value;
        LN* to_erase = current.second;
        if (current.second->next->next == nullptr)
            advance_cursors();
        ref_map->erase(to_erase->value.first);
        expected_mod_count = ref_map->mod_count;

        return to_return;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    std::string HashMap<KEY,T,thash>::Iterator::str() const {
        std::ostringstream answer;
        answer << ref_map->str();
//        if(current.second != nullptr)
//            answer << "(bin[" << current.first <<"],"<< current.second->value.first << ")";

        return answer.str();
    }

    template<class KEY,class T, int (*thash)(const KEY& a)>
    auto  HashMap<KEY,T,thash>::Iterator::operator ++ () -> HashMap<KEY,T,thash>::Iterator& {
        if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("ArrayMap::Iterator::operator ++");

//        if (current.first == ref_map->bins && current.second->next->next == nullptr) {
//            cout << "last key" << endl;
//            return *this;
//        }

        if(current.second == nullptr)
            return *this;

        if (can_erase)
            advance_cursors();
        else
            can_erase = true;  //current already indexes "one beyond" deleted value

        return *this;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    auto  HashMap<KEY,T,thash>::Iterator::operator ++ (int) -> HashMap<KEY,T,thash>::Iterator {
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("ArrayMap::Iterator::operator ++(int)");

//        if (current.first == ref_map->bins && current.second->next->next == nullptr) {
//            cout << "last key" << endl;
//            return *this;
//        }

        if (current.second == nullptr)
            return *this;

        Iterator to_return(*this);
        if (can_erase)
            advance_cursors();
        else
            can_erase = true;  //current already indexes "one beyond" deleted value

        return to_return;
    }



    template<class KEY,class T, int (*thash)(const KEY& a)>
    bool HashMap<KEY,T,thash>::Iterator::operator == (const HashMap<KEY,T,thash>::Iterator& rhs) const {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if (rhsASI == 0)
            throw IteratorTypeError("ArrayMap::Iterator::operator ==");
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("ArrayMap::Iterator::operator ==");
        if (ref_map != rhsASI->ref_map)
            throw ComparingDifferentIteratorsError("ArrayMap::Iterator::operator ==");


        return current.second == rhsASI->current.second;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    bool HashMap<KEY,T,thash>::Iterator::operator != (const HashMap<KEY,T,thash>::Iterator& rhs) const {
        const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
        if (rhsASI == 0)
            throw IteratorTypeError("ArrayMap::Iterator::operator !=");
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("ArrayMap::Iterator::operator !=");
        if (ref_map != rhsASI->ref_map)
            throw ComparingDifferentIteratorsError("ArrayMap::Iterator::operator !=");

        return current.second != rhsASI->current.second;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    pair<KEY,T>& HashMap<KEY,T,thash>::Iterator::operator *() const {
        if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("ArrayMap::Iterator::operator *");
        if (!can_erase || current.second == nullptr) {
            std::ostringstream where;
            where << current << " when size = " << ref_map->size();
            throw IteratorPositionIllegal("ArrayMap::Iterator::operator * Iterator illegal: "+where.str());
        }
        return current.second->value;
    }


    template<class KEY,class T, int (*thash)(const KEY& a)>
    pair<KEY,T>* HashMap<KEY,T,thash>::Iterator::operator ->() const {
        if (expected_mod_count != ref_map->mod_count)
            throw ConcurrentModificationError("ArrayMap::Iterator::operator ->");
        if (!can_erase || current.second == nullptr) {
            std::ostringstream where;
            where << current << " when size = " << ref_map->size();
            throw IteratorPositionIllegal("ArrayMap::Iterator::operator -> Iterator illegal: "+where.str());
        }
        return &current.second->value;
    }


}

#endif /* HASH_MAP_HPP_ */