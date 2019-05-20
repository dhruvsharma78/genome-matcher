#include "provided.h"
#include "Trie.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
using namespace std;

class GenomeMatcherImpl{
public:
    GenomeMatcherImpl(int minSearchLength);
    void addGenome(const Genome& genome);
    int minimumSearchLength() const;
    bool findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const;
    bool findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const;
private:
    int m_minimumSearchLength;
    vector<Genome> genomeLib;
    struct Tag{
        int index;
        int pos;
    };
    Trie<Tag> searchTrie;
};

bool genomeMatchSortHelper(const GenomeMatch& a, const GenomeMatch& b){
    if(a.percentMatch < b.percentMatch) return true;
    if(a.percentMatch > b.percentMatch) return false;
    
    return a.genomeName < b.genomeName;
}

GenomeMatcherImpl::GenomeMatcherImpl(int minSearchLength){
    m_minimumSearchLength = minSearchLength;
}

void GenomeMatcherImpl::addGenome(const Genome& genome){
    genomeLib.push_back(genome);
    for(int i=0; i<genome.length();i++){
        Tag* tg = new Tag();
        tg->index = genomeLib.size() -1;
        tg->pos = i;
        string x;
        if(genome.extract(i, minimumSearchLength(), x)){
            searchTrie.insert(x, *tg);
        }
    }
}

int GenomeMatcherImpl::minimumSearchLength() const{
    return m_minimumSearchLength;
}

bool GenomeMatcherImpl::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const{
    if((fragment.size() < minimumLength) || (minimumLength < minimumSearchLength())){
        return false;
    }
    
    matches.clear();
    matches.resize(genomeLib.size());
    
    vector<Tag> results = searchTrie.find(fragment.substr(0,minimumSearchLength()), exactMatchOnly);
    
    for(int i=0; i<results.size(); i++){
        string x;
        genomeLib[results[i].index].extract(results[i].pos, fragment.size(), x);
        int numDifferencesAfterN = 0;
        int numDifferencesBeforeN = 0;
        int lenMatch = 0;
        bool diffExists = false;
        for(int j=0; j<x.size(); j++){
            if(x[j] != fragment[j]){
                diffExists = true;
                if(j<=minimumLength-1){
                    numDifferencesBeforeN++;
                }else{
                    numDifferencesAfterN++;
                }
                if(!exactMatchOnly && numDifferencesBeforeN == 1 && numDifferencesAfterN == 0) lenMatch++;
            }else{
                if(numDifferencesAfterN == 0)
                    lenMatch++;
            }
        }
        if(!exactMatchOnly && numDifferencesBeforeN > 1){
            continue;
        }
        if(exactMatchOnly && numDifferencesBeforeN != 0){
            continue;
        }
        DNAMatch* dm = new DNAMatch;
        dm->genomeName = genomeLib[results[i].index].name();
        dm->position   = results[i].pos;
        dm->length     = lenMatch;
        
        if(dm->length > matches[results[i].index].length){
            matches[results[i].index] = *dm;
        }
        if(matches[results[i].index].length == dm->length){
            if(dm->position < matches[results[i].index].position){
                matches[results[i].index] = *dm;
            }
        }
    }
    
    
    for(vector<DNAMatch>::iterator it = matches.begin(); it != matches.end();){
        if((*it).genomeName == ""){
            it = matches.erase(it);
        }else{
            it++;
        }
    }
    
    if(matches.size() > 0){
        return true;
    }else{
        return false;
    }
    
    
}

bool GenomeMatcherImpl::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const{
    
    if(fragmentMatchLength < minimumSearchLength()) return false;
    
    map<string, int> count;
    for(int i=0; i<(query.length()/fragmentMatchLength); i++){
        string extractedSequence;
        query.extract(i*fragmentMatchLength, fragmentMatchLength, extractedSequence);
        std::vector<DNAMatch> matches;
        findGenomesWithThisDNA(extractedSequence, fragmentMatchLength, exactMatchOnly, matches);
        for(int j=0; j<matches.size(); j++){
            count[matches[j].genomeName]++;
        }
    }
    
    // Add to results
    results.clear();
    for(map<string, int>::iterator it = count.begin(); it != count.end(); it++){
        int total = (query.length()/fragmentMatchLength);
        double matchPercent = ((double)(it->second) / total) * 100;
        if(matchPercent >= matchPercentThreshold){
            GenomeMatch* gm = new GenomeMatch;
            gm->genomeName = it->first;
            gm->percentMatch = matchPercent;
            results.push_back(*gm);
        }
    }
    if(results.size() == 0) return false;
    
    //Sort
    sort(results.begin(), results.end(), &genomeMatchSortHelper);
    
    return true;
    
}

//******************** GenomeMatcher functions ********************************

// These functions simply delegate to GenomeMatcherImpl's functions.
// You probably don't want to change any of this code.

GenomeMatcher::GenomeMatcher(int minSearchLength)
{
    m_impl = new GenomeMatcherImpl(minSearchLength);
}

GenomeMatcher::~GenomeMatcher()
{
    delete m_impl;
}

void GenomeMatcher::addGenome(const Genome& genome)
{
    m_impl->addGenome(genome);
}

int GenomeMatcher::minimumSearchLength() const
{
    return m_impl->minimumSearchLength();
}

bool GenomeMatcher::findGenomesWithThisDNA(const string& fragment, int minimumLength, bool exactMatchOnly, vector<DNAMatch>& matches) const
{
    return m_impl->findGenomesWithThisDNA(fragment, minimumLength, exactMatchOnly, matches);
}

bool GenomeMatcher::findRelatedGenomes(const Genome& query, int fragmentMatchLength, bool exactMatchOnly, double matchPercentThreshold, vector<GenomeMatch>& results) const
{
    return m_impl->findRelatedGenomes(query, fragmentMatchLength, exactMatchOnly, matchPercentThreshold, results);
}
