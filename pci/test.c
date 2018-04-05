#ifndef TEST_C_INCLUDE
#define TEST_C_INCLUDE


//#######################################################
#endif /* TEST_C_INCLUDE */
#if !defined TEST_C_IMPLEMENTED && defined TEST_C_IMPL
#define TEST_C_IMPLEMENTED
//#######################################################




#if 0
void
test_bool_uint64_array_encoded_add(void)
{
    uint64_t array[2];
    for ( size_t i = 1; i < 72; i++ ) {
        size_t bits_count = i;        
        bool_uint64_array_encoded_add(array, bits_count);
    }    
}
#endif

#if 0
void
boolean_packing_unpacking_test(void)
{
    uint64_t array[2] = {0};
    size_t array_bits_count = 128;
    bool value = 1;
    for (size_t i = 0; i < 100; i++ ) {
        value = !value;
        boolean_pack_into_uint64_array(value, array, array_bits_count, i);
    }
    for (size_t i = 0; i < 100; i++ ) {
        boolean_unpack_from_uint64_array(array, array_bits_count, i);
    }

}
#endif






#endif /* TEST_C_IMPL */
