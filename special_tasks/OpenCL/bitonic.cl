__kernel void fast_sort(
    __global    int     *array,
    const       int     direction // 1 = Ascending, 0 = Descending
)
{
    int N = get_local_size(0);
    int i = get_global_id(0);

    for(int stage = 2; stage <= N; stage <<= 1)
    {
        for(int stride = stage >> 1; stride > 0; stride >>= 1)
        {
            int j = i ^ stride;
            if(j > i)
            {
                bool ascending = (((i & stage) == 0) == direction);
                int left = array[i];
                int right = array[j];
                if ((left > right) == ascending)
                {
                    array[i] = right;
                    array[j] = left;
                }
            }
            barrier(CLK_GLOBAL_MEM_FENCE);
        }
    }
}

__kernel void stride_sort(
    __global    int     *array,
    const       int     stage,
    const       int     stride,
    const       int     direction // 1 = Ascending, 0 = Descending
)
{
    int i = get_global_id(0);
    int j = i ^ stride;
    if(j > i)
    {
        bool ascending = (((i & stage) == 0) == direction);
        int left = array[i];
        int right = array[j];
        if ((left > right) == ascending)
        {
            array[i] = right;
            array[j] = left;
        }
    }
}
