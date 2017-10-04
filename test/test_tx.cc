#include <skycoin/coin/message.hh>
#include <skycoin/log.hh>

#include <assert.h>
#include <stdio.h>

int main() {
    
    FILE* fp = fopen("../test/data/givt.dat", "rb");
    fseek(fp, 0, SEEK_END);
    long len = ftello(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t* dat = new uint8_t[len];
    fread(dat, len, 1, fp);
    fclose(fp);

    skycoin::log().info("deserializing sample GIVT message at ../test/data/givt.dat");
    auto msg = skycoin::coin::message_factory("GIVT");
    auto res = msg->deserialize(dat, len);

    assert(res == msg->size+4);
    assert(res == size_t(len));

    auto* givt = skycoin::coin::message_cast<skycoin::coin::fourcc("GIVT")>(msg.get());
    skycoin::coin::transaction& t = givt->transactions[0];

    assert(t.outputs[0].coins == 161000000);
    assert(t.outputs[1].coins == 120000000);

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