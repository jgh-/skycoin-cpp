#include <skycoin/coin/message.hh>
#include <skycoin/log.hh>

#include <assert.h>
#include <stdio.h>

int main() {
    
    FILE* fp = fopen("../test/data/givb.dat", "rb");
    fseek(fp, 0, SEEK_END);
    long len = ftello(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t* dat = new uint8_t[len];
    fread(dat, len, 1, fp);
    fclose(fp);

    skycoin::log().info("deserializing sample GIVB message at ../test/data/givb.dat");
    auto msg = skycoin::coin::message_factory("GIVB");
    auto res = msg->deserialize(dat, len);
    
    assert(res == msg->size+4);

    auto& givb = static_cast<skycoin::coin::message<skycoin::coin::fourcc("GIVB")>&>(*msg);
    skycoin::coin::block& b = givb.blocks[0];

    assert(b.sequence == 3074);
    assert(b.transactions.size() == 1);
    assert(b.transactions[0].outputs.size() == 2);
    assert(b.transactions[0].outputs[0].coins == 156000000);
    assert(b.transactions[0].outputs[1].coins == 15000000);
    assert(b.transactions[0].outputs[1].hours = 1095);

    skycoin::log().info("OK");
    skycoin::log().info("Serializing same message.");
    std::vector<uint8_t> outdata;
    res = msg->serialize(outdata);
    assert(outdata.size() == size_t(len));
    skycoin::log().info("OK");
    skycoin::log().info("Comparing bitstreams.");
    for(size_t i = 0 ; i < outdata.size(); i++) {
        assert(dat[i] == outdata[i]);
    }
    skycoin::log().info("OK");
    delete [] dat;
    return 0;
}