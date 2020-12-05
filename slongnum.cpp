
#ifndef LONG_NUM_S_IMPL
#define LONG_NUM_S_IMPL

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
LongNumCore::index_t SignedLongNum<storage_t,base,digits>::serialize( void* v ) const {
    char *mem = reinterpret_cast<char*>(v);
    LongNumCore::index_t off = parent_t::serialize(v);
    memcpy( mem+off, &positive, sizeof(positive) );
    return off + sizeof(positive);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
LongNumCore::index_t SignedLongNum<storage_t,base,digits>::deserialize( const void* v ){
    LongNumCore::index_t off = parent_t::deserialize(v);
    if( off ){
        memcpy( &positive, reinterpret_cast<const char*>(v) + off, sizeof(positive) );
        return off + sizeof(positive);
    }
    return 0;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void SignedLongNum<storage_t,base,digits>::placement_add( const SignedLongNum& a, const SignedLongNum& b ){
    if( a.isPositive() ){
        if( b.isPositive() ){
            parent_t::placement_add(a,b);
            abs();
        }else{
            positive = a.toUnsigned() >= b.toUnsigned();
            if( positive ){
                parent_t::placement_sub(a,b);
            }else{
                parent_t::placement_sub(b,a);
            }
        }
    }else{
        if( b.isPositive() ){
            positive = b.toUnsigned() >= a.toUnsigned();
            if( positive ){
                parent_t::placement_sub(b,a);
            }else{
                parent_t::placement_sub(a,b);
            }
        }else{
            parent_t::placement_add(a,b);
            negate();
        }
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void SignedLongNum<storage_t,base,digits>::placement_sub( const SignedLongNum& a, const SignedLongNum& b ){
    if( a.isPositive() ){
        if( b.isPositive() ){
            positive = a.toUnsigned() >= b.toUnsigned();
            if( positive ){
                parent_t::placement_sub(a,b);
            }else{
                parent_t::placement_sub(b,a);
            }
        }else{
            parent_t::placement_add(a,b);
            abs();
        }
    }else{
        if( b.isPositive() ){
            parent_t::placement_add(a,b);
            negate();
        }else{
            positive = b.toUnsigned() >= a.toUnsigned();
            if( positive ){
                parent_t::placement_sub(b,a);
            }else{
                parent_t::placement_sub(a,b);
            }
        }
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void SignedLongNum<storage_t,base,digits>::placement_mul( const SignedLongNum& a, const SignedLongNum& b ){
    positive = !(a.positive ^ b.positive);
    parent_t::placement_mul(a,b);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void SignedLongNum<storage_t,base,digits>::placement_div( const SignedLongNum& a, const SignedLongNum& b ){
    positive = !(a.positive ^ b.positive);
    parent_t::placement_div(a,b);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void SignedLongNum<storage_t,base,digits>::placement_mod( const SignedLongNum& a, const SignedLongNum& b ){
    positive = a.positive;
    parent_t::placement_mod(a,b);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator>=( storage_t v ) const {
    
    if( v >= 0 ){
        if( isPositive() ){
            return toUnsigned() >= v;
        }else{
            return false;
        }
    }

    if( isPositive() ){
        return true;
    }else{
        return toUnsigned() <= -v;
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator<=( storage_t v ) const {

    if( v >= 0 ){
        if( isPositive() ){
            toUnsigned() <= v;
        }else{
            return true;
        }
    }

    if( isPositive() ){
        return false;
    }else{
        return toUnsigned() >= -v;
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator>( storage_t v ) const {

    if( v >= 0 ){
        if( isPositive() ){
            return toUnsigned() > v;
        }else{
            return false;
        }
    }

    if( isPositive() ){
        return true;
    }else{
        return toUnsigned() < -v;
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator<( storage_t v ) const {

    if( v >= 0 ){
        if( isPositive() ){
            return toUnsigned() < v;
        }else{
            return true;
        }
    }

    if( isPositive() ){
        return false;
    }else{
        return toUnsigned() > -v;
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator>=( const SignedLongNum& v ) const {
    if( isPositive() ){
        if( v.isPositive() ){
            return toUnsigned() >= v.toUnsigned();
        }else{
            return true;
        }
    }else{
        if( v.isPositive() ){
            return false;
        }else{
            return toUnsigned() <= v.toUnsigned();
        }
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator<=( const SignedLongNum& v ) const {
    if( isPositive() ){
        if( v.isPositive() ){
            return toUnsigned() <= v.toUnsigned();
        }else{
            return false;
        }
    }else{
        if( v.isPositive() ){
            return true;
        }else{
            return toUnsigned() >= v.toUnsigned();
        }
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator>( const SignedLongNum& v ) const {
    if( isPositive() ){
        if( v.isPositive() ){
            return toUnsigned() > v.toUnsigned();
        }else{
            return true;
        }
    }else{
        if( v.isPositive() ){
            return false;
        }else{
            return toUnsigned() < v.toUnsigned();
        }
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator<( const SignedLongNum& v ) const {
    if( isPositive() ){
        if( v.isPositive() ){
            return toUnsigned() < v.toUnsigned();
        }else{
            return false;
        }
    }else{
        if( v.isPositive() ){
            return true;
        }else{
            return toUnsigned() > v.toUnsigned();
        }
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator==( const SignedLongNum& v ) const {
    return positive == positive && toUnsigned() == v.toUnsigned();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator!=( const SignedLongNum& v ) const {
    return positive != positive || toUnsigned() != v.toUnsigned();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator==( storage_t v ) const {
    
    if( v >= 0 ){
        return positive && toUnsigned() == v;
    }

    return !positive && toUnsigned() == -v;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool SignedLongNum<storage_t,base,digits>::operator!=( storage_t v ) const {
    
    if( v >= 0 ){
        return !positive || toUnsigned() != v;
    }

    return positive || toUnsigned() != v;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
SignedLongNum<storage_t,base,digits>::SignedLongNum( decltype(Uninitialized) X ) :
    parent_t{X}
{}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
SignedLongNum<storage_t,base,digits>::SignedLongNum() :
    parent_t{},
    positive{true}
{}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
SignedLongNum<storage_t,base,digits>::SignedLongNum( storage_t v ) :
    parent_t{ v < 0 ? -v : v },
    positive{ v >= 0 }
{}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
void SignedLongNum<storage_t,base,digits>::zero(){
    positive = true;
    parent_t::zero();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
void SignedLongNum<storage_t,base,digits>::negate(){
    positive = false;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
void SignedLongNum<storage_t,base,digits>::flip(){
    positive = !positive;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
void SignedLongNum<storage_t,base,digits>::abs(){
    positive = true;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
LongNumCore::index_t SignedLongNum<storage_t,base,digits>::getSerializationBufferSize() const {
    return sizeof(positive) + parent_t::getSerializationBufferSize();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
SignedLongNum<storage_t,base,digits>& SignedLongNum<storage_t,base,digits>::operator=( storage_t v ){
    positive = v >= 0;
    parent_t::operator=( positive ? v : -v );
    return *this;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
storage_t SignedLongNum<storage_t,base,digits>::toInt() const {
    return positive ? parent_t::toInt() : -1 * parent_t::toInt();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
double SignedLongNum<storage_t,base,digits>::toDouble() const {
    return positive ? parent_t::toDouble() : -1.0 * parent_t::toDouble();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
const UnsignedLongNum<storage_t,base,digits>& SignedLongNum<storage_t,base,digits>::toUnsigned() const {
    return *this;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
void SignedLongNum<storage_t,base,digits>::placement_shl( const SignedLongNum& v, LongNumCore::index_t n ){
    positive = v.positive;
    parent_t::placement_shl(v,n);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
void SignedLongNum<storage_t,base,digits>::placement_shr( const SignedLongNum& v, LongNumCore::index_t n ){
    positive = v.positive;
    parent_t::placement_shr(v,n);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
bool SignedLongNum<storage_t,base,digits>::isPositive() const {
    return positive;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits>
bool SignedLongNum<storage_t,base,digits>::isNegative() const {
    return !positive;
}

#endif
