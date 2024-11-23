#include<iostream>
#include <fstream> 
#include <stdexcept>
#include <chrono>
#include "byteio.h"
#include "bitio.h"
#include "Model.h"
using namespace std;

template<typename INPUT, typename OUTPUT, typename MODEL> class OPG_COM
{
    typedef typename MODEL::CODE_VALUE CODE_VALUE;
    typedef typename MODEL::prob prob;
public:
    OPG_COM(INPUT& input, OUTPUT& output, MODEL& model)
        : m_input(input),m_output(output),m_model(model)
    {
    }
    int operator()()
    {
        CODE_VALUE low = 0; 
        CODE_VALUE high = MODEL::MAX_CODE;  
        int bits_to_follow = 0;
        for (; ;) {
            int c = m_input.getByte();
            if (c == -1) {
                c = 256;
            }
            prob p = m_model.getProbability(c);

            CODE_VALUE range = high - low + 1;
            high = low + (range * p.high / p.count) - 1; 
            low = low + (range * p.low / p.count);
            for (; ;) {
                if (high < MODEL::ONE_HALF) {
                    put_bit_plus_pending(0, bits_to_follow);
                }
                else if (low >= MODEL::ONE_HALF) {
                    put_bit_plus_pending(1, bits_to_follow);
                    low -= MODEL::ONE_HALF;
                    high -= MODEL::ONE_HALF;
                }
                else if (low >= MODEL::ONE_FOURTH && high < MODEL::THREE_FOURTHS) {
                    bits_to_follow++;
                    low -= MODEL::ONE_FOURTH;
                    high -= MODEL::ONE_FOURTH;
                }
                else {
                    break;
                }
                low += low;
                high += high + 1;
                
            }
            if (c == 256)
                break;
        }
        return 0;
    }
    void put_bit_plus_pending(bool bit, int& bits_to_follow)
    {
        m_output.put_bit(bit);
        for (int i = 0; i < bits_to_follow; i++) {
            m_output.put_bit(!bit);
        }
        bits_to_follow = 0;
    }
private:
    OUTPUT& m_output;
    INPUT& m_input;
    MODEL& m_model;
};

template<typename INPUT, typename OUTPUT, typename MODEL>
int encode(INPUT& source, OUTPUT& target, MODEL& model)
{
    input_bytes<INPUT> in(source);
    output_bits<OUTPUT> out(target);
    OPG_COM<input_bytes<INPUT>, output_bits<OUTPUT>, MODEL> c(in, out, model);
    return c();
}

template<typename INPUT, typename OUTPUT, typename MODEL> class OPG_DEC
{
    typedef typename MODEL::CODE_VALUE CODE_VALUE;
    typedef typename MODEL::prob prob;
public:
    OPG_DEC(INPUT& input, OUTPUT& output, MODEL& model)
        : m_input(input), m_output(output), m_model(model)
    {
    }
    int operator()()
    {
        int low = 0;
        int high = MODEL::MAX_CODE;
        int value = 0;
        for (int i = 0; i < MODEL::CODE_VALUE_BITS; i++) {
            value <<= 1; 
            value += m_input.get_bit() ? 1 : 0;
        }
        for (; ; ) {
            CODE_VALUE range = high - low + 1;
            CODE_VALUE scaled_value = ((value - low + 1) * m_model.getCount() - 1) / range;
            int c;

            prob p = m_model.getChar(scaled_value, c);
            if (c == 256)
                break;
            m_output.putByte(c);
            high = low + (range * p.high) / p.count - 1;
            low = low + (range * p.low) / p.count;
            for (; ; ) {
                if (high < MODEL::ONE_HALF) {
                    ;
                }
                else if (low >= MODEL::ONE_HALF) {
                    low -= MODEL::ONE_HALF;
                    high -= MODEL::ONE_HALF;
                    value -= MODEL::ONE_HALF;
                }
                else if (low >= MODEL::ONE_FOURTH && high < MODEL::THREE_FOURTHS) {
                    low -= MODEL::ONE_FOURTH;
                    high -= MODEL::ONE_FOURTH;
                    value -= MODEL::ONE_FOURTH;
                }
                else {
                    break;
                }
                low += low;
                high += high + 1;
                value <<= 1;
                value += m_input.get_bit() ? 1 : 0;
            }
            
        }
        return 0;
    }
private:
    OUTPUT& m_output;
    INPUT& m_input;
    MODEL& m_model;
};

template<typename INPUT, typename OUTPUT, typename MODEL>
int decode(INPUT& source, OUTPUT& target, MODEL& model)
{
    input_bits<INPUT> in(source, MODEL::CODE_VALUE_BITS);
    output_bytes<OUTPUT> out(target);
    OPG_DEC<input_bits<INPUT>, output_bytes<OUTPUT>, MODEL> d(in, out, model);
    return d();
}
double SG(const string& input_file,const string& comp_file)
{
    double res;
    ifstream in(input_file.c_str(), ifstream::binary);
    if (!in) {
        cout << "Error name inputfile" << endl;
        exit(-1);
    }
    ifstream comp(comp_file.c_str(), ifstream::binary);
    if (!comp) {
        cout << "Error name comp_file" << endl;
        exit(-2);
    }
    in.seekg(0, ios::end);
    comp.seekg(0, ios::end);
    auto in_length = in.tellg();
    auto comp_length = comp.tellg();
    if (static_cast<long long>(in_length) == 0)
        res = 8.0;
    else
        res = comp_length * 8.0 / in_length;
    return res;
}


int main() {
    system("chcp 1251");
    system("cls");
    char filename[80] = { 0 };
    char filenameOutput[80] = { 0 };
    char filenameOutputDecode[80] = { 0 };
    cout << "Input: ";
    cin >> filename;
    cout << "Output: ";
    cin >> filenameOutput;
    cout << "OutputDecode: ";
    cin >> filenameOutputDecode;
    
    ifstream input(filename, ifstream::binary);
    ofstream output(filenameOutput, ofstream::binary);
    Model<> q1;
    q1.dump("cmodel");

    cout << "encodeing " << endl;
    auto t1 = chrono::high_resolution_clock::now();
    encode(input, output, q1);
    auto t2 = chrono::high_resolution_clock::now();
    cout << q1.m_bytesProcessed << endl;
    input.close();
    output.close();
    cout << "Время на кодирование " << chrono::duration_cast<chrono::milliseconds>(t2 - t1).count() <<"ms" << endl;
    cout << "Степень сжатия " << SG(filename, filenameOutput) << endl;

    cout << endl;
    ifstream inputDe(filenameOutput, ifstream::binary);
    ofstream outputDe(filenameOutputDecode, ofstream::binary);
    Model<> q2;
    //q2.dump("cmodel");

    cout << "decoding " << endl;
    auto t3 = chrono::high_resolution_clock::now();
    decode(inputDe, outputDe, q2);
    auto t4 = chrono::high_resolution_clock::now();
    cout << q2.m_bytesProcessed << endl;
    inputDe.close();
    outputDe.close();
    cout << "Время на декодирование " << chrono::duration_cast<chrono::milliseconds>(t4 - t3).count()<< "ms" << endl;

    
}