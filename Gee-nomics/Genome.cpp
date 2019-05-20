#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <istream>
using namespace std;

class GenomeImpl{
public:
    GenomeImpl(const string& nm, const string& sequence);
    static bool load(istream& genomeSource, vector<Genome>& genomes);
    int length() const;
    string name() const;
    bool extract(int position, int length, string& fragment) const;
private:
    string m_name;
    int m_length;
    string m_sequence;
};

GenomeImpl::GenomeImpl(const string& nm, const string& sequence){
    m_name = nm;
    m_length = sequence.size();
    m_sequence = sequence;
}

bool GenomeImpl::load(istream& genomeSource, vector<Genome>& genomes) {
    string s;
    string sequence = "";
    string name     = "";
    Genome* gx;
    int currentGenomeNumber = -1;
    int currentLineNumber = 0;
    bool hadBases = false;
    while (getline(genomeSource, s)){
        if(currentLineNumber == 0 && s[0] != '>') return false;
        if(s.size() == 0) return false;
        if(s[0] == '>'){
            if(currentGenomeNumber != -1){
                if(!hadBases) return false;
                gx = new Genome(name, sequence);
                genomes.push_back(*gx);
            }
            hadBases = false;
            name     = s.substr(1);
            if(name.size() == 0) return false;
            sequence = "";
            currentGenomeNumber++;
        }else{
            for(int i=0; i<s.size(); i++){
                s[i] = toupper(s[i]);
                if(s[i]!='A' && s[i]!='C' && s[i]!='T' && s[i]!='G' && s[i]!='N') return false;
            }
            sequence += s;
            hadBases = true;
        }
        currentLineNumber++;
    }
    if(!hadBases) return false;
    gx = new Genome(name, sequence);
    genomes.push_back(*gx);
    return true;
}

int GenomeImpl::length() const
{
    return m_length;
}

string GenomeImpl::name() const
{
    return m_name;
}

bool GenomeImpl::extract(int position, int length, string& fragment) const
{
    if((position+length) > this->length()){
        return false;
    }
    
    fragment = "";
    for(int i=position; i<(position+length); i++){
        fragment += m_sequence[i];
    }
    
    return true;
}

//******************** Genome functions ************************************

// These functions simply delegate to GenomeImpl's functions.
// You probably don't want to change any of this code.

Genome::Genome(const string& nm, const string& sequence)
{
    m_impl = new GenomeImpl(nm, sequence);
}

Genome::~Genome()
{
    delete m_impl;
}

Genome::Genome(const Genome& other)
{
    m_impl = new GenomeImpl(*other.m_impl);
}

Genome& Genome::operator=(const Genome& rhs)
{
    GenomeImpl* newImpl = new GenomeImpl(*rhs.m_impl);
    delete m_impl;
    m_impl = newImpl;
    return *this;
}

bool Genome::load(istream& genomeSource, vector<Genome>& genomes)
{
    return GenomeImpl::load(genomeSource, genomes);
}

int Genome::length() const
{
    return m_impl->length();
}

string Genome::name() const
{
    return m_impl->name();
}

bool Genome::extract(int position, int length, string& fragment) const
{
    return m_impl->extract(position, length, fragment);
}
