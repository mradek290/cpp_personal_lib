
#ifndef LONG_NUM_F_IMPL
#define LONG_NUM_F_IMPL

template<
    LongNumCore::index_t alph_sz,
    LongNumCore::byte_t (*toByte)( LongNumCore::byte_t ),
    LongNumCore::byte_t (*toAlph)( LongNumCore::byte_t ),
    bool (*isAlph)( LongNumCore::byte_t ) >
constexpr LongNumCore::index_t LongNumFormatInfo<alph_sz,toByte,toAlph,isAlph>::GetAlphabetSize(){
    return alph_sz;
}

template<
    LongNumCore::index_t alph_sz,
    LongNumCore::byte_t (*toByte)( LongNumCore::byte_t ),
    LongNumCore::byte_t (*toAlph)( LongNumCore::byte_t ),
    bool (*isAlph)( LongNumCore::byte_t ) >
LongNumCore::byte_t LongNumFormatInfo<alph_sz,toByte,toAlph,isAlph>::AlphabetToByte( LongNumCore::byte_t c ){
    return toByte(c);
}

template<
    LongNumCore::index_t alph_sz,
    LongNumCore::byte_t (*toByte)( LongNumCore::byte_t ),
    LongNumCore::byte_t (*toAlph)( LongNumCore::byte_t ),
    bool (*isAlph)( LongNumCore::byte_t ) >
LongNumCore::byte_t LongNumFormatInfo<alph_sz,toByte,toAlph,isAlph>::ByteToAlphabet( LongNumCore::byte_t c ){
    return toAlph(c);
}

template<
    LongNumCore::index_t alph_sz,
    LongNumCore::byte_t (*toByte)( LongNumCore::byte_t ),
    LongNumCore::byte_t (*toAlph)( LongNumCore::byte_t ),
    bool (*isAlph)( LongNumCore::byte_t ) >
bool LongNumFormatInfo<alph_sz,toByte,toAlph,isAlph>::IsAlphabet( LongNumCore::byte_t c ){
    return isAlph(c);
}

#endif
