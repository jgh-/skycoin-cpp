#include <skycoin/coin/message.hh>
#include <skycoin/log.hh>

#include <assert.h>
#include <stdio.h>

int main() {

    FILE* fp = fopen("../test/data/givp.dat", "rb");
    fseek(fp, 0, SEEK_END);
    long len = ftello(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t* dat = new uint8_t[len];
    fread(dat, len, 1, fp);
    fclose(fp);

    skycoin::log().info("deserializing sample GIVP message at ../test/data/givp.dat");
    auto msg = skycoin::coin::message_factory("GIVP");
    size_t res = msg->deserialize(dat, len);
    auto* givp = skycoin::coin::message_cast<skycoin::coin::fourcc("GIVP")>(msg.get());
    assert(res == size_t(len));
    assert(givp != nullptr);
    assert(givp->peers.size() == 13);
    assert(givp->peers[0].address == "tcp://121.41.103.148:6000");
    assert(givp->peers[5].address == "tcp://197.97.221.117:6000");
    assert(givp->peers[8].address == "tcp://124.33.3.5:7200");
    assert(givp->peers[12].address == "tcp://176.9.47.13:6000");

    skycoin::log().info("OK.");
    skycoin::log().info("Serializing same message.");
    std::vector<uint8_t> outdata;
    res = msg->serialize(outdata);

    assert( (res == msg->size+4) && (res == size_t(len)));

    skycoin::log().info("OK.");
    skycoin::log().info("Comparing bitstreams.");
    for(size_t i = 0 ; i < outdata.size(); i++) {
        assert(dat[i] == outdata[i]);
    }
    skycoin::log().info("OK");
    delete [] dat;
}