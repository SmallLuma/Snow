#include "Snow.h"
#include "Snow/Debug.h"
using namespace std;
using namespace Snow;

string ResVal::Trim(const std::string& s)
{
    string t;
    FOR_EACH(p,s.begin(),s.end())
        if(*p != ' ' && *p != '\t' && *p != '\0' && *p != '\r')
            t+=*p;
    return t;
}


bool ResVal::Load(const std::string& f)
{
    ResFile in(f);
    if(!in.Success()){
        PNT("Snow::ResVal::Load:Can't Open ResVal " + f);
        return false;
    }
    Uint32 pos = 0;
    while(pos<in.Size()){
        string line = GetLine(in,pos);

        string trimline = Trim(line);

        if(trimline.empty() || trimline[0] == '#') continue;

        //GetType

        int type = 0;

        {
            string s = line.substr(0,line.find(' '));
            s = Trim(s);
            UpperCase(s);
            if(s == "INT") type = 0;
            else if(s == "FLOAT") type = 1;
            else if(s == "STR") type = 2;
            else if(s == "INCLUDE"){
                string fileName;
                fileName = line.substr(line.find(' ')+1,line.length() - line.find(' ') - 1);
                if(*(--(fileName.end())) == '\r')
                    fileName = fileName.substr(0,fileName.length() - 1);
                Load(fileName);
                continue;
            }
            else continue;
        }

        //GetName
        string name;
        {
            Uint32 pos1 = line.find(' ');
            Uint32 pos2 = line.find('=');
            name = line.substr(pos1+1,pos2-pos1-1);
            name = Trim(name);
            UpperCase(name);
        }

        //GetVal
        string val;
        {
            Uint32 pos1 = line.find('=');
            val = line.substr(pos1+1,line.length()-pos1-1);
        }

        //ProcVal
        if(type != 2) val = Trim(val);
        switch(type){
        case 0:
            m_ints[name] = atoi(val.c_str());break;
        case 1:
            m_flts[name] = (float)atof(val.c_str());break;
        case 2:
            string sval;
            bool st = false;
            for(unsigned int i = 0;i < val.size();i++){
                if(st){
                    if(val[i] != '\"') sval+= val[i];
                    else {m_strs[name] = sval;continue;}
                }else if(val[i] == '\"') st = true;
            }
            break;
        }
    }
    return true;
}

const string & ResVal::Str(std::string s)
{
    UpperCase(s);
    if(m_strs.count(s) == 0){
        PNT(string("Snow::ResVal::Str:Can't Find String Value ")+s+" in "<<this);
        return m_nullstr;
    };
    return m_strs[s];
}

float ResVal::Float(std::string s)
{
    UpperCase(s);
    if(m_flts.count(s) == 0){
        PNT(string("Snow::ResVal::Float:Can't Find Float Value ")+s+" in "<<this);
        return 0;
    }
    return m_flts[s];
}

Sint32 ResVal::Int(std::string s)
{
    UpperCase(s);
    if(m_ints.count(s) == 0){
        PNT(string("Snow::ResVal::Int:Can't Find Int Value ")+s+" in "<<this);
        return 0;
    }
    return m_ints[s];
}
