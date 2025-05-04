#include "oram_client.h"
#include "oram_storage.h"
#include "oram_protocol.h"
#include "ringlwe.h"

void init_globals() {
    INV_N = modinv(N, Q);
}

void test_oram_access_success() {
    cout << "[Test] ORAM Client can access inserted entry\n";

    auto [pk, sk] = keygen();

    constexpr size_t Z = 4;
    constexpr size_t N_buckets = 1024;

    OramStorage<Z> storage(N_buckets);
    OramClient client(storage.num_levels());

    // Insert an entry manually at the bottom level, bucket 0
    uint64_t target_id = 42;
    Poly val(N, 0); val[0] = target_id; val[1] = 99;
    size_t target_level = storage.num_levels() - 1;
    storage.get_bucket(target_level, 0).entries[0] = OramEntry(target_id, val);

    // Access it
    auto result = client.access(target_id, pk, sk, storage);
    if (result)
        cout << "Found id=" << result->id << " value=" << result->value[1] << "\n";
    else
        cout << "Not found\n";
}

void test_oram_access_success() {
    cout << "[Test] ORAM Client can access inserted entry\n";

    auto [pk, sk] = keygen();

    constexpr size_t N = 32;
    constexpr size_t Z = 5;     // pretty sure Z = log N?

    OramStorage<Z> storage(N);
    OramClient client(storage.num_levels());

    std::vector<Bucket<Z>> arr;
}

int main() {
    init_globals();
    test_oram_access_success();
    test_oram_access_failure();
    return 0;
}
