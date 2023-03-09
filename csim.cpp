#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <iterator>
#include <algorithm>
#include <iomanip>

class FlexCache {
    public:
        int nWords; // C (cache capcity 16)
        int wordsPerBlock; // b
        int nWay; // N

        FlexCache(int x, int y, int z) // constructor with parameters
        { 
            nWords = x;
            wordsPerBlock = y;
            nWay = z;
        }
};

void evaluateCache(std::string problem, int array[], FlexCache FlexCache, int arrLen)
{
    int numBlocks = round(FlexCache.nWords/(float)FlexCache.wordsPerBlock); // nunmber of blocks
    int numSets = round(numBlocks/(float)FlexCache.nWay); // number of sets
    int blockOffsetBits = round(log2(FlexCache.wordsPerBlock)); // number of block offsets
    int numSetBits = round(log2(numSets)); // num of bits for set
    int byteOffset, blockOffset, set, tag;
    int setBitsBinary = pow(2, numSetBits) - 1;
    int blockOffsetBitsBinary = pow(2, blockOffsetBits) - 1;
    std::string hitFalse = "Hit:false";
    std::string hitTrue = "Hit:true";
    std::vector<int> vector = { -1, -1, -1, -1, -1, -1, -1, -1, // vector of 16
                                -1, -1, -1, -1, -1, -1, -1, -1}; // vecotr index = set

    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    std::cout << problem << std::endl;
    std::vector<int> tempVector;
    for (int j = 1; j < 3; j++) {

        int tempArray[arrLen];
        std::copy_n(array, arrLen, tempArray);
        int numHits = 0; 
        int numMisses = 0; 

        for (int i = 0; i < arrLen; i++) 
        {
            std::cout << "Address:" << std::hex << std::setw(8) << std::left << tempArray[i] << std::dec;

            byteOffset = (tempArray[i] & 3); // get the byte offset (last 2 bits)
            tempArray[i] = (tempArray[i] >> 2); // shift word adress to the right 2 times to get next element

            if (FlexCache.wordsPerBlock == 1) 
                std::cout << std::setw(14) << std::left << "BlkOfs:0";
            
            else 
            {
                blockOffset = (tempArray[i] & blockOffsetBitsBinary);
                std::cout << "BlkOfs:" << std::setw(7) << std::left << blockOffset;
                tempArray[i] = (tempArray[i] >> blockOffsetBits);
            }

            set = (tempArray[i] & setBitsBinary); // gets the set bits 82^(numsetbit) 2^3 - 1 = 7 = 0x111
            std::cout << "Set:" << std::setw(4) << std::left << set;

            tempArray[i] = (tempArray[i] >> numSetBits); // shift word adress num of set bits times to get the tag next
            tag = tempArray[i];
            std::cout << "Tag:"  << std::setw(4) << std::left << tag;

            if (vector[set] == tag) 
            {
                std::cout << std::setw(10) << std::left << hitTrue;
                numHits += 1;
            }
            else 
            {
                int hit = 0;
                for (std::vector<int>::iterator iter = tempVector.begin(); iter!= tempVector.end(); )
                { // iterate through to check for duplicates
                    if (*iter == tag && numSets == 1 && FlexCache.nWay == 8 && numBlocks == 8)
                    {
                        iter = tempVector.erase(iter);
                        hit = 1;
                    } else iter++;
                }

                if (hit == 1 && numSets == 1 && FlexCache.nWay == 8 && numBlocks == 8)
                {
                    std::cout << std::setw(10) << std::left << hitTrue;
                    tempVector.insert(tempVector.end(), tag);
                    numHits += 1;
                }
                else if (vector[set] == -1) {
                    vector[set] = tag;
                    
                    if (tempVector.size() >= numBlocks) 
                    {
                        tempVector.erase(tempVector.begin());
                        tempVector.insert(tempVector.end(), tag);
                    }
                    else tempVector.push_back(tag);
                    std::cout << std::setw(10) << std::left << hitFalse;
                    numMisses += 1;
                }
                else {
                    vector[set] = tag;
                    if (tempVector.size() >= numBlocks) 
                    {
                        tempVector.erase(tempVector.begin());
                        tempVector.insert(tempVector.end(), tag);
                    }
                    else if (tempVector.size() > 7 && numBlocks == 8)
                    {
                        tempVector.erase(tempVector.begin());
                        tempVector.insert(tempVector.end(), tag);
                    }
                    else tempVector.push_back(tag);
                    std::cout << std::setw(10) << std::left << hitFalse;
                    numMisses += 1;
                }
            }

            std::cout << "      Cache:";

            if ((numSets == 16 || numSets == 8 || numSets == 4) && FlexCache.nWay == 1) 
            {
                for (int i = 0; i < numSets; i++) 
                { 
                    std::cout << "[";
                    for (int j = 0; j < FlexCache.nWay; j++) 
                    {
                        if (vector[i] == -1) {std::cout << "";}
                        else {std::cout << vector[i];} 
                    }
                    std::cout << "]";
                }
            }

            else if (numSets == 1 && FlexCache.nWay == 16 || FlexCache.nWay == 8) 
            {
                std::cout << "[";
                for (std::vector<int>::iterator it = tempVector.begin(); it!= tempVector.end(); it++)
                {
                    if (it == tempVector.end()-1) std::cout << *it << "";
                    else std::cout << *it << ", ";
                }
                std::cout << "]";
            }

            else if (numSets == 8 && FlexCache.nWay == 2 && numBlocks == 16) 
            {
                for (int i = 0; i < numSets; i++) 
                { 
                    int checker = 0;
                    std::cout << "[";
                    for (int j=0; j < FlexCache.nWay; j++)
                    {
                        int index = (i * FlexCache.nWay) + j;
                        if (vector[i] == -1) {std::cout << "";}
                        else {
                            if (checker == 1) {
                                tempVector.at(i) = tempVector.at(i+8);
                                tempVector.at(i+8) = tag;
                                std::cout << tempVector[i];
                                if (j == 0) std::cout << ", ";
                                std::cout << tempVector[i+8];
                            }

                            else if (tempVector.size() > 8 || tempVector.size() == 16) 
                            {
                                std::cout << tempVector[i];
                                if (tempVector[i+8] > -1) std::cout << ", " << tempVector[i+8];
                                if (tempVector.size() == 16) checker = 1;
                            }
                            else {std::cout << tempVector[i];}
                        } 
                        if (j < 8) break;
                        
                    }
                    std::cout << "]";
                }
            }
            else if (numSets == 4 && FlexCache.nWay == 2 && numBlocks == 8) 
            {
                for (int i = 0; i < numSets; i++) 
                { 
                    ///int checker = 0;
                    std::cout << "[";
                    for (int j=0; j < FlexCache.nWay; j++)
                    {
                        int index = (i * FlexCache.nWay) + j;
                        if (vector[i] == -1) {std::cout << "";}
                        else 
                        {
                            if (tempVector.size() > 4 || tempVector.size() == 8) 
                            {
                                std::cout << tempVector[i];
                                if (tempVector[i+4] > -1) std::cout << ", " << tempVector[i+4];
                            }
                            else {std::cout << tempVector[i];}
                        } 
                        if (j < 8) break;
                    }
                    std::cout << "]";
                }
            }
            
            std::cout << "\n";;
        }

        float hitPercent = ((numHits/(float)arrLen) * 100);
        float missPercent = ((numMisses/(float)arrLen) * 100);

        std::cout << "Pass " << j << " hits:" << numHits << " = " << hitPercent << "%" << std::endl;
        std::cout << "Pass " << j << " misses:" << numMisses << " = " << missPercent << "%\n" << std::endl;
    }
}

void p810()
{
    int p810m[] = { 0x74, 0xA0, 0x78, 0x38C, 0xAC, 
                    0x84, 0x88, 0x8C, 0x7C, 0x34, 
                    0x38, 0x13C, 0x388, 0x18C };
    int arrLen = (sizeof(p810m)/ sizeof(int));

    evaluateCache("Problem 8.10a", p810m, FlexCache(16, 1, 1), arrLen);
    evaluateCache("Problem 8.10b", p810m, FlexCache(16, 2, 8), arrLen);
    evaluateCache("Problem 8.10c", p810m, FlexCache(16, 2, 2), arrLen);
    evaluateCache("Problem 8.10d", p810m, FlexCache(16, 4, 1), arrLen);
} 

void p809()
{
    int p809m[] = { 0x40, 0x44, 0x48, 0x4C, 0x70, 0x74, 0x78, 0x7C,
                    0x80, 0x84, 0x88, 0x8C, 0x90, 0x94, 0x98, 0x9C,
                    0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C,
                    0x20 };
    
    int arrLen = (sizeof(p809m) / sizeof(int)); // sizeof gets size in bytes so divide by size of an int
    evaluateCache("Problem 8.9a", p809m, FlexCache(16, 1, 1), arrLen); // direct map: nWays = 1
    evaluateCache("Problem 8.9b", p809m, FlexCache(16, 1, 16), arrLen); // full associative cache: nWays = nWords
    evaluateCache("Problem 8.9c", p809m, FlexCache(16, 1, 2), arrLen);  // two way set associative: nWays = 2
    evaluateCache("Problem 8.9d", p809m, FlexCache(16, 2, 1), arrLen);
}

int main()
{
    p809();
    p810();
    return 0;
}