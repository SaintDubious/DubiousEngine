#include "AC3DFileReader.h"
#include "FilePath.h"

#include <fstream>
#include <cassert>

//////////////////////////////////////////////////////////////
using Dubious::Utility::AC3DModel;
using Dubious::Utility::AC3DModelPtr;
using Dubious::Utility::AC3DFilePtr;
using Dubious::Utility::AC3DMaterial;
using Dubious::Utility::AC3DFileReader;
using Dubious::Utility::FilePath;

////////////////////////////////////////////////////////////////////////////////////
// info on the file format can be found at:
// http://www.ac3d.org/

namespace
{

////////////////////////////////////////////////////////////////////////////////////
// material line looks like:
// MATERIAL "" rgb 1 1 1  amb 0.2 0.2 0.2  emis 0 0 0  spec 0.5 0.5 0.5  shi 10  trans 0
// (I've alread read the MATERIAL token)
// This is hard coded to do nothing right now.  I don't care so much about materials
// at the moment.
AC3DMaterial ReadMaterial( std::ifstream& Input )
{
    std::string Str;
    float Junk;
    float Red, Green, Blue;
    Input >> Str
          >> Str >> Red >> Green >> Blue
          >> Str >> Junk >> Junk >> Junk
          >> Str >> Junk >> Junk >> Junk
          >> Str >> Junk >> Junk >> Junk
          >> Str >> Junk 
          >> Str >> Junk;
    return AC3DMaterial( AC3DMaterial::Color( Red, Green, Blue ) );
}

const std::string SURF			= "SURF";
const std::string MAT			= "mat";
const std::string REFS			= "refs";

////////////////////////////////////////////////////////////////////////////////////
void ReadSurfaces( std::ifstream& Input, AC3DModel::SurfaceVector& Surfaces )
{
    int NumSurfaces;
    Input >> NumSurfaces;
    for (int i=0; i<NumSurfaces; ++i) {
        bool bReadSurface = false;
        AC3DModel::Surface NewSurface;
        std::string Token;
        while( !bReadSurface ){
            Input >> Token;

            if (Token == SURF) {
                std::string s;
                Input >> s;
            } 
            else if (Token == MAT) {
                Input >> NewSurface.MaterialIndex;
            } 
            else if (Token == REFS) {
                bReadSurface = true;
                int	SurfaceCount;
                Input >> SurfaceCount;
                if (SurfaceCount != 3)
                    throw std::runtime_error( "Model includes surface with more then 3 vertices" );
                int	Index;
                float TexX, TexY;
                Input >> Index >> TexX >> TexY;
                NewSurface.p0 = Index;
                Input >> Index >> TexX >> TexY;
                NewSurface.p1 = Index;
                Input >> Index >> TexX >> TexY;
                NewSurface.p2 = Index;
            } 
            else 
                throw std::runtime_error( "Found invalid Surface token" );
        }
        Surfaces.push_back( NewSurface );
    }
}

////////////////////////////////////////////////////////////////////////////////////
const std::string NAME			= "name";
const std::string DATA			= "data";
const std::string TEXTURE		= "texture";
const std::string TEXREP		= "texrep";
const std::string ROT			= "rot";
const std::string LOC			= "loc";
const std::string URL			= "url";
const std::string NUMVERT		= "numvert";
const std::string NUMSURF		= "numsurf";
const std::string KIDS			= "kids";
const std::string CREASE		= "crease";
const std::string OBJECT		= "OBJECT";
const std::string MATERIAL		= "MATERIAL";


////////////////////////////////////////////////////////////////////////////////////
AC3DModelPtr ReadModel( std::ifstream& Input, bool& LOCPresent )
{
    AC3DModelPtr pModel( new AC3DModel() );
    std::string Token;
    std::string s;
    int i;
    float f;
    float x, y, z;
    std::string ObjectType;
    Input >> ObjectType;
    while (true) {
        Input >> Token;
        if (Token == NAME) {
            Input >> pModel->Name();
        } 
        else if (Token == DATA) {
            Input >> i;
        } 
        else if (Token == TEXTURE) {
            Input >> s;
        } 
        else if (Token == TEXREP) {
            Input >> f >> f;
        } 
        else if (Token == ROT) {
            throw std::runtime_error( "System does not support rotation matrixes" );
        } 
        else if (Token == CREASE) {
            Input >> f;
        } 
        else if (Token == LOC) {
            Input >> x >> y >> z;
            pModel->Offset() = AC3DModel::Point( x, y, z );
            LOCPresent = true;
        } 
        else if (Token == URL) {
            Input >> s;
        } 
        else if (Token == NUMVERT) {
            int NumVertices;
            Input >> NumVertices;
            for (i=0; i<NumVertices; i++) {
                Input >> x >> y >> z;
                pModel->Points().push_back( AC3DModel::Point( x, y, z ) );
            }
        } 
        else if (Token == NUMSURF) {
            ReadSurfaces( Input, pModel->Surfaces() );
        } 
        else if (Token == KIDS) {
            int NumKids;
            Input >> NumKids;
            for (i=0; i<NumKids; ++i) {
                std::string SubToken;
                Input >> SubToken;
                if (SubToken != OBJECT)
                    throw std::runtime_error( "Kid specified, but not found" );
                AC3DModelPtr pKid = ReadModel( Input, LOCPresent );
                pModel->Kids().push_back( pKid );
            }
            break;
        } 
        // NOTE: it turns out that an AC3D file can have other stuff written in here too.
        // In that case the full line is ignored.  This is an issue because Blender outputs
        // names of its mesh objects.  This code will probably barf if there's more then
        // just a single token on the line... but until then..
    }

    return pModel;
}

}

////////////////////////////////////////////////////////////////////////////////////
AC3DFilePtr AC3DFileReader::ReadFile( const FilePath& FileName )
{
    std::ifstream InputFile( FileName.FullPath().c_str() );
    
    std::string Token;
    InputFile >> Token;
    if (!InputFile.good())
        throw std::runtime_error( "Couldn't read first token" );
    if (Token != "AC3Db")
        throw std::runtime_error( "AC3DFileReader passed invalid file" );

    bool loc = false;
    AC3DMaterialVector Materials;
    AC3DModelPtr pModel;
    while (true) {
        InputFile >> Token;
        if (InputFile.eof())
            break;
        if (Token == MATERIAL)
            Materials.push_back( ReadMaterial( InputFile ) );
        else if (Token == OBJECT) {
            pModel = ReadModel( InputFile, loc );
            break;
        } 
        else 
            throw std::runtime_error( "Encountered an unknown token" );
    }
    if (!pModel)
        throw std::runtime_error( "AC3D file contained no object" );
    assert( loc && "You forgot to re-center the child components" );

    return AC3DFilePtr( new AC3DFile( Materials, pModel ) );
}

