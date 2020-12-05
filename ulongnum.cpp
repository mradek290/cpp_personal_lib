
#ifndef LONG_NUM_U_IMPL
#define LONG_NUM_U_IMPL

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::div( storage_t n ){
    
    storage_t carry = 0;
    index_t i = cnt;

    while( i-- > 0 ){

        storage_t t = base * carry + dat[i];
        dat[i] = t / n;
        carry = t % n;
    }

    clear_leading_zeros();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::clear_leading_zeros(){

    while( cnt > 0 && dat[cnt-1] == 0 )
        --cnt;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_div( const UnsignedLongNum& v, storage_t n ){
    
    storage_t carry = 0;
    index_t i = v.cnt;

    while( i-- > 0 ){

        storage_t t = base * carry + v.dat[i];
        dat[i] = t / n;
        carry = t % n;
    }

    clear_leading_zeros();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::mod( storage_t n ){
    
    storage_t carry = 0;
    index_t i = cnt;

    while( i-- > 0 ){

        storage_t t = base * carry + dat[i];
        carry = t % n;
    }

    *this = carry;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_mod( const UnsignedLongNum& v, storage_t n ){
    
    storage_t carry = 0;
    index_t i = v.cnt;

    while( i-- > 0 ){

        storage_t t = base * carry + v.dat[i];
        carry = t % n;
    }

    *this = carry;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
LongNumCore::index_t UnsignedLongNum<storage_t,base,digits>::deserialize( const void* v ){
    
    const char* mem = reinterpret_cast<const char*>(v);
    number_t base_ref;

    memcpy( &base_ref, mem, sizeof(base_ref) );
    mem += sizeof(base_ref);

    valid = base_ref == base;
    if( valid ){

        memcpy( &cnt, mem, sizeof(cnt) );
        mem += sizeof(cnt);

        memcpy( &valid, mem, sizeof(valid) );
        mem += sizeof(valid);

        memcpy( dat, mem, cnt * sizeof(storage_t) );
        return mem - reinterpret_cast<const char*>(v) + cnt*sizeof(storage_t);
    }

    return 0;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
LongNumCore::index_t UnsignedLongNum<storage_t,base,digits>::getSerializationBufferSize() const {
    return sizeof(number_t) + sizeof(cnt) + sizeof(valid) + cnt * sizeof(storage_t);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
LongNumCore::index_t UnsignedLongNum<storage_t,base,digits>::serialize( void* v ) const {

    char* mem = reinterpret_cast<char*>(v);
    const number_t base_ref = base;

    memcpy( mem, &base_ref, sizeof(base_ref) );
    mem += sizeof(base_ref);

    memcpy( mem, &cnt, sizeof(cnt) );
    mem += sizeof(cnt);

    memcpy( mem, &valid, sizeof(valid) );
    mem += sizeof(valid);

    memcpy( mem, dat, cnt*sizeof(storage_t) );
    return mem - reinterpret_cast<char*>(v) + cnt*sizeof(storage_t);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
storage_t& UnsignedLongNum<storage_t,base,digits>::operator[]( index_t n ){
    return dat[n];
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
const storage_t& UnsignedLongNum<storage_t,base,digits>::operator[]( index_t n ) const {
    return dat[n];
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_mul( const UnsignedLongNum& a, const UnsignedLongNum& b ){
    
    zero();
    if( a.isZero() || b.isZero() )
        return;
    
    UnsignedLongNum tmp = Uninitialized;

    for( index_t i = 0; i < b.cnt; ++i ){

        tmp.placement_mul(a, b.dat[i] );
        tmp.shl(i);
        this->add(tmp);
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::isZero() const {
    return cnt == 1 && dat[0] == 0;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_mod( const UnsignedLongNum& a, const UnsignedLongNum& b ){
    
    if( b.cnt >= a.cnt ){

        if( b.cnt == a.cnt ){

            number_t q = b_search( a, b, *this, 1, base ) - 1;
            UnsignedLongNum t = Uninitialized;
            t.placement_mul(b,q);
            this->placement_sub(a,t);
            return;
        }

        *this = a;
        return;
    }

    index_t v;
    number_t q;
    UnsignedLongNum r = a;
    UnsignedLongNum d = Uninitialized;

    do{
        v = r.cnt - b.cnt - 1;
        d.placement_shl(b,v);
        q = b_search( r, d, *this, 1, base*base ) - 1;

        d.mul(q);
        r.sub(d);

    }while( r.cnt > b.cnt );

    if( r.cnt >= b.cnt ){

        q = b_search( r, b, *this, 1, base ) - 1;
        this->placement_mul(b,q);
        r.sub(*this);
    }

    *this = r;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_div( const UnsignedLongNum& a, const UnsignedLongNum& b ){
    

    if( b.cnt >= a.cnt ){
        *this = b_search( a, b, *this, 1, base ) - 1;
        return;
    }

    index_t v;
    number_t q;
    UnsignedLongNum r = a;
    UnsignedLongNum d = Uninitialized;
    UnsignedLongNum tmp = Uninitialized;
    this->zero();

    do{
        v = r.cnt - b.cnt - 1;
        d.placement_shl(b,v);
        q = b_search( r, d, tmp, 1, base*base ) - 1;

        d.mul(q);
        r.sub(d);

        tmp = q;
        tmp.shl(v);

        this->add(tmp);

    }while( r.cnt > b.cnt );

    if( r.cnt >= b.cnt ){

        q = b_search( r, b, tmp, 1, base ) - 1;
        this->add(q);
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::sub( storage_t v ){
    
    valid = true;
    storage_t carry = 0;
    index_t i = 0;

    while( v > 0 ){
        storage_t delta = dat[i] - (v%base) - carry;
        dat[i] = ( base + delta ) % base;
        carry = ( base - delta )/(base+1);
        v /= base;
        ++i;
    }

    while( i < cnt && carry ){
        storage_t delta = dat[i] - carry;
        dat[i] = ( base + delta ) % base;
        carry = ( base - delta )/(base+1);
        ++i;
    }

    clear_leading_zeros();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_sub( const UnsignedLongNum& a, storage_t v ){

    storage_t carry = 0;
    valid = true;

    cnt = 0;
    while( v > 0 ){
        storage_t delta = a.dat[cnt] - (v%base) - carry;
        dat[cnt] = ( base + delta ) % base;
        carry = ( base - delta )/(1+base);
        v /= base;
        ++cnt;
    }

    while( cnt < a.cnt ){
        storage_t delta = a.dat[cnt] - carry;
        dat[cnt] = ( base + delta ) % base;
        carry = ( base - delta )/(base+1);
        ++cnt;
    }

    clear_leading_zeros();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_sub( const UnsignedLongNum& a, const UnsignedLongNum& b ){

    storage_t carry = 0;
    valid = a.cnt >= b.cnt;

    cnt = 0;
    do{
        storage_t delta = a.dat[cnt] - b.dat[cnt] - carry;
        dat[cnt] = ( base + delta ) % base;
        carry = ( base - delta )/(1+base);
    }while( ++cnt < b.cnt );

    while( cnt < a.cnt ){

        storage_t delta = a.dat[cnt] - carry;
        dat[cnt] = ( base + delta ) % base;
        carry = ( base - delta )/(base+1);
        ++cnt;
    }

    while( cnt > 1 && dat[cnt-1] == 0 )
        --cnt;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::sub( const UnsignedLongNum& v ){
    
    valid = cnt > v.cnt;
    storage_t carry = 0;
    index_t i = 0;

    do{
        storage_t delta = dat[i] - v.dat[i] - carry;
        dat[i] = ( base + delta ) % base;
        carry = ( base - delta )/(base+1);
    }while( ++i < v.cnt );

    while( i < cnt && carry ){

        storage_t delta = dat[i] - carry;
        dat[i] = ( base + delta ) % base;
        carry = ( base - delta )/(base+1);
        ++i;
    }

    clear_leading_zeros();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator>=( const UnsignedLongNum& v ) const {
    
    if( cnt == v.cnt ){

        index_t i = cnt;
        while( --i > 0 && dat[i] == v.dat[i] );
        return dat[i] >= v.dat[i];
    }

    return cnt > v.cnt;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator<=( const UnsignedLongNum& v ) const {
    
    if( cnt == v.cnt ){

        index_t i = cnt;
        while( --i > 0 && dat[i] == v.dat[i] );
        return dat[i] <= v.dat[i];
    }

    return cnt < v.cnt;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
LongNumCore::number_t UnsignedLongNum<storage_t,base,digits>::b_search( 
    const UnsignedLongNum& num, const UnsignedLongNum& div, UnsignedLongNum& tmp, number_t lo, number_t hi ){

    if( hi == lo ) return hi;

    number_t x = (hi+lo)/2;
    tmp.placement_mul(div,x);

    if( tmp > num )
        return b_search(num,div,tmp,lo,x);
    else
        return b_search(num,div,tmp,x+1,hi);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator<( const UnsignedLongNum& v ) const {
    
    if( cnt == v.cnt ){

        index_t i = cnt-1;
        do{
            if( v.dat[i] != dat[i] )
                return dat[i] < v.dat[i];
        }while( i-- > 0 );

        return false;
    }

    return cnt < v.cnt;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
storage_t UnsignedLongNum<storage_t,base,digits>::toInt() const {

    storage_t x = 0;
    storage_t p = 1;
    for( auto i = 0; i < cnt; ++i ){
        x += dat[i] * p;
        p *= base;
    }

    return x;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
double UnsignedLongNum<storage_t,base,digits>::toDouble() const {

    double x = 0;
    double p = 1;
    for( auto i = 0; i < cnt; ++i ){
        x += dat[i] * p;
        p *= base;
    }

    return x;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_add( const UnsignedLongNum& v, storage_t n ){
    
    *this = v;
    add(n);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_mul( const UnsignedLongNum& v, storage_t n ){
    
    if( !n || v.isZero() ){
        zero();
    }else{

        index_t i = 0;
        storage_t carry = 0;

        while( i < v.cnt ){

            carry += v.dat[i]*n;
            dat[i] = carry % base;
            carry /= base;
            ++i;
        }

        cnt = v.cnt;
        valid = cnt < digits;
        while( carry > 0 && valid ){

            dat[cnt++] = carry % base;
            carry /= base;
            valid = cnt < digits;
        }
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_add( const UnsignedLongNum& a, const UnsignedLongNum& b ){
    
    storage_t carry = 0;
    valid = a.valid && b.valid;
    cnt = 0;

    while( cnt < a.cnt && cnt < b.cnt ){

        carry += a.dat[cnt] + b.dat[cnt];
        dat[cnt++] = carry % base;
        carry /= base;
    }

    while( cnt < a.cnt ){

        carry += a.dat[cnt];
        dat[cnt++] = carry % base;
        carry /= base;
    }

    while( cnt < b.cnt ){

        carry += b.dat[cnt];
        dat[cnt++] = carry % base;
        carry /= base;
    }

    while( carry > 0 ){

        dat[cnt++] = carry % base;
        carry /= base;
    }

    if( cnt == 0 )
        zero();
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator==( const UnsignedLongNum& v ) const {
    
    if( cnt != v.cnt ) return false;
    return memcmp( dat, v.dat, cnt*sizeof(storage_t) ) == 0;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator!=( const UnsignedLongNum& v ) const {
    
    if( cnt != v.cnt ) return true;
    return memcmp( dat, v.dat, cnt*sizeof(storage_t) ) != 0;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator>( const UnsignedLongNum& v ) const {
    
    if( cnt == v.cnt ){

        index_t i = cnt-1;
        do{
            if( v.dat[i] != dat[i] )
                return dat[i] > v.dat[i];
        }while( i-- > 0 );
    }

    return v.cnt < cnt;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_shr( const UnsignedLongNum& v, index_t n ){

    valid = v.valid;
    cnt = v.cnt - n;
    memcpy( dat, v.dat + n, cnt * sizeof(storage_t) );
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::placement_shl( const UnsignedLongNum& v, index_t n ){

    
    memcpy( dat+n, v.dat, sizeof(storage_t) * v.cnt );
    memset( dat, 0, n*sizeof(storage_t) );
    cnt = v.cnt + n;
    valid = v.valid;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::shl( index_t n ){
    
    memmove( dat+n, dat, sizeof(storage_t)*cnt );
    memset( dat, 0, n*sizeof(storage_t) );
    cnt += n;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::shr( index_t n ){
    
    memmove( dat, dat + n, (cnt-n)*sizeof(storage_t) );
    memset( dat + cnt - n, 0, (cnt - n) * sizeof(storage_t) );
    cnt -= n;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::add( storage_t v ){

    storage_t carry = v;
    index_t i = 0;

    while( i < cnt && carry > 0 ){

        carry += dat[i];
        dat[i] = carry % base;
        carry /= base;
        ++i;
    }

    while( carry > 0 && valid ){

        dat[cnt] = carry % base;
        carry /= base;
        valid = ++cnt < digits;
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::mul( storage_t v ){

    index_t i = 0;
    storage_t carry = 0;

    while( i < cnt ){

        carry += dat[i] * v;
        dat[i] = carry % base;
        carry /= base;
        ++i;
    }

    valid &= cnt < digits;
    while( carry > 0 && valid ){

        dat[cnt++] = carry % base;
        carry /= base;
        valid = cnt < digits;
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::add( const UnsignedLongNum& v ){

    index_t i = 0;
    storage_t carry = 0;

    while( i < cnt && i < v.cnt ){

        carry += dat[i] + v.dat[i];
        dat[i] = carry % base;
        carry /= base;

        ++i;
    }

    while( i < v.cnt ){

        carry += v.dat[i];
        dat[cnt++] = carry % base;
        carry /= base;

        ++i;
    }

    valid &= cnt < digits;
    while( carry > 0 && valid ){

        dat[cnt++] = carry % base;
        carry /= base;
        valid = cnt < digits;
    }
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
template< class format >
void UnsignedLongNum<storage_t,base,digits>::assign( const byte_t* str ){

    index_t str_sz = strlen(str);
    const byte_t* s = str + str_sz;
    index_t pwr = 1;
    UnsignedLongNum t = Uninitialized;
    index_t shifts = 0;
    storage_t tmp = 0;

    zero();

    while( s-- > str && format::IsAlphabet(*s) ){

        byte_t c = format::AlphabetToByte(*s);
        if( pwr*format::GetAlphabetSize() > base ){

            t = tmp;
            t.shl(shifts++);
            this->add(t);

            tmp = 0;
            pwr = 1;

            ++s;

        }else{

            tmp += c*pwr;
            pwr *= format::GetAlphabetSize();
        }
    }

    t = tmp;
    t.shl(shifts);
    this->add(t);
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
UnsignedLongNum<storage_t,base,digits>& UnsignedLongNum<storage_t,base,digits>::operator=( storage_t v ){

    if( v == 0 ){
        zero();
    }else{

        valid = true;
        cnt = 0;
        dat[0] = 0;

        while( v > 0 ){

            dat[cnt++] = v % base;
            v /= base;
        }
    }

    return *this;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
UnsignedLongNum<storage_t,base,digits>::UnsignedLongNum( decltype(Uninitialized) ){

}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
UnsignedLongNum<storage_t,base,digits>::UnsignedLongNum( storage_t v ){
    *this = v;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
UnsignedLongNum<storage_t,base,digits>::UnsignedLongNum() :
    valid{true},
    cnt{1}
{
    memset( dat, 0, digits * sizeof(storage_t) );
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
LongNumCore::index_t UnsignedLongNum<storage_t,base,digits>::getMagnitude() const {
    return cnt;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
constexpr LongNumCore::number_t UnsignedLongNum<storage_t,base,digits>::getBase(){
    return base;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
constexpr LongNumCore::index_t UnsignedLongNum<storage_t,base,digits>::getCapacity(){
    return digits;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
void UnsignedLongNum<storage_t,base,digits>::zero(){

    valid = true;
    cnt = 1;
    memset( dat, 0, digits * sizeof(storage_t) );
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator>( storage_t v ) const {

    UnsignedLongNum t = v;
    return *this > t;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator<( storage_t v ) const {

    UnsignedLongNum t = v;
    return *this < t;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator>=( storage_t v ) const {

    UnsignedLongNum t = v;
    return *this >= t;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator<=( storage_t v ) const {

    UnsignedLongNum t = v;
    return *this <= t;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator!=( storage_t v ) const {

    UnsignedLongNum t = v;
    return *this != t;
}

template< class storage_t, LongNumCore::number_t base, LongNumCore::index_t digits >
bool UnsignedLongNum<storage_t,base,digits>::operator==( storage_t v ) const {

    index_t i = 0;
    do{
        if( dat[i++] != v % base )
            return false;
        v /= base;

    }while( i < cnt && v > 0 );

    return i == cnt && v == 0;
}

#endif
