#include <iostream>
#include <vector>

constexpr int num_ranks = 16;
constexpr int num_nvl_ranks = 8;
constexpr int num_ranks_per_group = 4;
constexpr int num_buffers_per_peer = num_ranks / num_ranks_per_group;

std::vector<int> get_rank_info(int rank, int num_ranks)
{
    int rdma_rank = rank / num_nvl_ranks;
    int nvl_rank = rank % num_nvl_ranks;
    std::vector<int> res;
    for (int i = 0; i < num_ranks_per_group; ++i)
    {
        int peer_rank = rank / num_ranks_per_group * num_ranks_per_group +  (rank + i) % num_ranks_per_group;
        res.push_back(peer_rank);
        res.push_back(rdma_rank* num_nvl_ranks + ( peer_rank % num_nvl_ranks + num_ranks_per_group) % num_nvl_ranks);
        if (num_buffers_per_peer > 2)
        {
            res.push_back((1 - rdma_rank) * num_nvl_ranks + peer_rank % num_nvl_ranks);
            res.push_back((1 - rdma_rank) * num_nvl_ranks + (peer_rank % num_nvl_ranks + num_ranks_per_group) % num_nvl_ranks);
        }
    }
    return res;
}


std::vector<int> get_buffer_index(int rank, int num_ranks)
{
    std::vector<int> buffer_index_of_ranks(num_ranks, -1);

    int rdma_rank = rank / num_nvl_ranks;
    int nvl_rank = rank % num_nvl_ranks;
    for (int i = 0; i < num_ranks_per_group; ++i)
    {
        int peer_rank = rank / num_ranks_per_group * num_ranks_per_group +  (rank + i) % num_ranks_per_group;
        buffer_index_of_ranks[peer_rank] = i * num_buffers_per_peer;
        buffer_index_of_ranks[rdma_rank* num_nvl_ranks + ( peer_rank % num_nvl_ranks + num_ranks_per_group) % num_nvl_ranks] = i * num_buffers_per_peer+ 1;
        if (num_buffers_per_peer > 2)
        {
            buffer_index_of_ranks[(1 - rdma_rank) * num_nvl_ranks + peer_rank % num_nvl_ranks] = i * num_buffers_per_peer + 2; 
            buffer_index_of_ranks[(1 - rdma_rank) * num_nvl_ranks + (peer_rank % num_nvl_ranks + num_ranks_per_group) % num_nvl_ranks] = i * num_buffers_per_peer + 3;
        }
    }
    return buffer_index_of_ranks;
}


void print(const std::vector<int> &v)
{
    for (int i = 0; i < num_ranks_per_group; ++i)
    {
        for (int j = 0; j < num_buffers_per_peer; ++j)
            std::cout << v[i * num_buffers_per_peer + j] << ' ';
        std::cout << std::endl;
    }
}



int main(void)
{

    std::vector<int> buffer_index_of_ranks(num_ranks);
    for (int i = 0; i < num_ranks; ++i)
    {
        std::cout << "======================= rank " << i << " =======================" << std::endl;
        std::cout << "rank_info: " << std::endl;
        std::vector<int> rank_info = get_rank_info(i, num_ranks);
        print(rank_info);
        std::vector<int> buffer_index_of_ranks = get_buffer_index(i, num_ranks);
        std::cout << "buffer_index_of_ranks: " << std::endl;
        print(buffer_index_of_ranks);
    }
    return 0;
}
