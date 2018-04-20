#ifndef MEM_LAYOUT_C_INCLUDE
#define MEM_LAYOUT_C_INCLUDE



typedef uint32_t packed_bool;


#define BOOL_PACKED_ARRAY_NELEMS(bitscount, typeof_arraymember)  \
    ((((bitscount) - 1) / (sizeof(typeof_arraymember) * 8)) + 1)

#define BOOL_PACKED_ARRAY_SIZE(bitscount, typeof_arraymember)           \
    BOOL_PACKED_ARRAY_NELEMS(bitscount, typeof_arraymember) * sizeof(typeof_arraymember)

#define BOOL_PACK_INTO_ARRAY(val, bit_index, array, array_num_members, typeof_arraymember) \
    do {                                                                \
        assert(bit_index < (array_num_members) * sizeof(typeof_arraymember) * 8); \
        size_t ___index___ = ((size_t)bit_index) / ( sizeof(typeof_arraymember) * 8); \
        (array)[___index___] =                                          \
            (typeof_arraymember)((array)[___index___]                   \
                                 & ~((typeof_arraymember)1 << ((size_t)(bit_index) - ___index___ * sizeof(typeof_arraymember) * 8))) \
            | (typeof_arraymember)(val) << ((size_t)(bit_index) - ___index___ * sizeof(typeof_arraymember) * 8); \
    } while(0)

#define BOOL_UNPACK_FROM_ARRAY(bit_index, array, array_num_members, typeof_arraymember) \
    (assert(bit_index < (array_num_members) * sizeof(typeof_arraymember) * 8), \
     (((array)[((size_t)(((size_t)bit_index) / ( sizeof(typeof_arraymember) * 8)))] \
       & ((typeof_arraymember) 1 << (((size_t)(bit_index) - ((size_t)(((size_t)bit_index) \
                                                                      / ( sizeof(typeof_arraymember) * 8))) * sizeof(typeof_arraymember) * 8)))) \
      >> ((size_t)(bit_index) - ((size_t)(((size_t)bit_index) / ( sizeof(typeof_arraymember) * 8))) * sizeof(typeof_arraymember) * 8))) \




//#######################################################
#endif /* MEM_LAYOUT_C_INCLUDE */
#if !defined MEM_LAYOUT_C_IMPLEMENTED && defined MEM_LAYOUT_C_IMPL
#define MEM_LAYOUT_C_IMPLEMENTED
//#######################################################







#endif /* MEM_LAYOUT_C_IMPL */
