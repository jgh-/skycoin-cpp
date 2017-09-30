#include <skycoin/coin/block.hh>

#include <assert.h>
#include <stdio.h>

int main() {
    
    {
        FILE* fp = fopen("../test/data/givb.dat", "rb");
        fseek(fp, 0, SEEK_END);
        long len = ftello(fp);
        fseek(fp, 12, SEEK_SET);
        uint8_t* dat = new uint8_t[len];
        fread(dat, len, 1, fp);
        fclose(fp);
        skycoin::coin::block b;
        b.deserialize(dat, len);
        delete [] dat;

        assert(b.sequence == 3074);
        assert(b.transactions.size() == 1);
        assert(b.transactions[0].outputs.size() == 2);
        assert(b.transactions[0].outputs[0].coins == 156000000);
        assert(b.transactions[0].outputs[1].coins == 15000000);
        assert(b.transactions[0].outputs[1].hours = 1095);
    }

    return 0;
}