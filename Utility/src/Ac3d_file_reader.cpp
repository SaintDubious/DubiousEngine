#include "Ac3d_file_reader.h"
#include "File_path.h"

#include <fstream>
#include <cassert>
#include <tuple>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

//////////////////////////////////////////////////////////////
// info on the file format can be found at:
// http://www.ac3d.org/

namespace
{

//////////////////////////////////////////////////////////////
// material line looks like:
// MATERIAL "" rgb 1 1 1  amb 0.2 0.2 0.2  emis 0 0 0  spec 0.5 0.5 0.5  shi 10  trans 0
// (I've alread read the MATERIAL token)
// This is hard coded to do nothing right now.  I don't care so much about materials
// at the moment.
Ac3d_material read_material( std::ifstream& Input )
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
    return Ac3d_material( Ac3d_material::Color( Red, Green, Blue ) );
}

const std::string SURF			= "SURF";
const std::string MAT			= "mat";
const std::string REFS			= "refs";

//////////////////////////////////////////////////////////////
void read_surfaces( std::ifstream& input, std::vector<Ac3d_model::Surface>& surfaces )
{
    int num_surfaces;
    input >> num_surfaces;
    for (int i=0; i<num_surfaces; ++i) {
        bool read_surface = false;
        Ac3d_model::Surface new_surface;
        while( !read_surface ){
            std::string token;
            input >> token;
            if (token == SURF) {
                std::string s;
                input >> s;
            } 
            else if (token == MAT) {
                input >> new_surface.material_index;
            } 
            else if (token == REFS) {
                read_surface = true;
                int	count;
                input >> count;
                if (count != 3) {
                    throw std::runtime_error( "Model includes surface with more then 3 vertices" );
                }
                int	index;
                float tex_x, tex_y;
                input >> index >> tex_x >> tex_y;
                new_surface.p0 = index;
                input >> index >> tex_x >> tex_y;
                new_surface.p1 = index;
                input >> index >> tex_x >> tex_y;
                new_surface.p2 = index;
            } 
            else {
                throw std::runtime_error( "Found invalid Surface token" );
            }
        }
        surfaces.push_back( new_surface );
    }
}

//////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////
std::unique_ptr<Ac3d_model> read_model( std::ifstream& input, bool& loc_present )
{
    std::unique_ptr<Ac3d_model> model = std::make_unique<Ac3d_model>();
    std::string object_type;
    input >> object_type;
    while (true) {
        std::string s;
        float f;
        float x, y, z;
        std::string token;
        input >> token;
        if (token == NAME) {
            input >> model->name();
        } 
        else if (token == DATA) {
            int i;
            input >> i;
        } 
        else if (token == TEXTURE) {
            input >> s;
        } 
        else if (token == TEXREP) {
            input >> f >> f;
        } 
        else if (token == ROT) {
            throw std::runtime_error( "System does not support rotation matrixes" );
        } 
        else if (token == CREASE) {
            input >> f;
        } 
        else if (token == LOC) {
            input >> x >> y >> z;
            model->offset() = Math::Local_point( x, y, z );
            loc_present = true;
        } 
        else if (token == URL) {
            input >> s;
        } 
        else if (token == NUMVERT) {
            int NumVertices;
            input >> NumVertices;
            for (int i=0; i<NumVertices; i++) {
                input >> x >> y >> z;
                model->points().push_back( Math::Local_point( x, y, z ) );
            }
        } 
        else if (token == NUMSURF) {
            read_surfaces( input, model->surfaces() );
        } 
        else if (token == KIDS) {
            int num_kids;
            input >> num_kids;
            for (int i=0; i<num_kids; ++i) {
                std::string sub_token;
                input >> sub_token;
                if (sub_token != OBJECT) {
                    throw std::runtime_error( "Kid specified, but not found" );
                }
                model->push_kid( std::move(read_model( input, loc_present )) );
            }
            break;
        } 
        // NOTE: it turns out that an AC3D file can have other stuff written in here too.
        // In that case the full line is ignored.  This is an issue because Blender outputs
        // names of its mesh objects.  This code will probably barf if there's more then
        // just a single token on the line... but until then..
    }

    return model;
}

}

//////////////////////////////////////////////////////////////
std::unique_ptr<const Ac3d_file> Ac3d_file_reader::read_file( const File_path& file_name )
{
    std::ifstream input_file( file_name.full_path().c_str() );
    if (!input_file.good()) {
        throw std::runtime_error( "Couldn't open AC3D file" );
    }
    
    std::string token;
    input_file >> token;
    if (!input_file.good())
        throw std::runtime_error( "Couldn't read first token" );
    if (token != "AC3Db")
        throw std::runtime_error( "AC3DFileReader passed invalid file" );

    bool loc = false;
    std::vector<Ac3d_material> materials;
    std::unique_ptr<Ac3d_model> model;
    while (true) {
        input_file >> token;
        if (input_file.eof())
            break;
        if (token == MATERIAL)
            materials.push_back( read_material( input_file ) );
        else if (token == OBJECT) {
            model = read_model( input_file, loc );
            break;
        } 
        else {
            throw std::runtime_error( "Encountered an unknown token" );
        }
    }
    if (!model) {
        throw std::runtime_error( "AC3D file contained no object" );
    }
    assert( loc && "You forgot to re-center the child components" );

    return std::make_unique<const Ac3d_file>( std::move(materials), std::move(model) );
}

//////////////////////////////////////////////////////////////
namespace {
Ac3d_model::Surface build_surface( int p0, int p1, int p2 )
{
    Ac3d_model::Surface new_surface;
    new_surface.material_index = 0;
    new_surface.p0 = p0;
    new_surface.p1 = p1;
    new_surface.p2 = p2;
    return new_surface;
}

std::unique_ptr<Ac3d_model> build_cube( float edge_length )
{
    std::unique_ptr<Ac3d_model> model = std::make_unique<Ac3d_model>();
    // The code for this comes from looking at an AC3D file
    // for a cube.
    model->points().push_back( Math::Local_point( -edge_length, -edge_length, -edge_length ) );
    model->points().push_back( Math::Local_point(  edge_length, -edge_length, -edge_length ) );
    model->points().push_back( Math::Local_point(  edge_length, -edge_length,  edge_length ) );
    model->points().push_back( Math::Local_point( -edge_length, -edge_length,  edge_length ) );
    model->points().push_back( Math::Local_point( -edge_length,  edge_length,  edge_length ) );
    model->points().push_back( Math::Local_point(  edge_length,  edge_length,  edge_length ) );
    model->points().push_back( Math::Local_point(  edge_length,  edge_length, -edge_length ) );
    model->points().push_back( Math::Local_point( -edge_length,  edge_length, -edge_length ) );

    model->surfaces().push_back( build_surface( 6, 2, 1 ) );
    model->surfaces().push_back( build_surface( 2, 6, 5 ) );
    model->surfaces().push_back( build_surface( 4, 0, 3 ) );
    model->surfaces().push_back( build_surface( 0, 4, 7 ) );
    model->surfaces().push_back( build_surface( 6, 0, 7 ) );
    model->surfaces().push_back( build_surface( 0, 6, 1 ) );
    model->surfaces().push_back( build_surface( 2, 4, 3 ) );
    model->surfaces().push_back( build_surface( 4, 2, 5 ) );
    model->surfaces().push_back( build_surface( 4, 6, 7 ) );
    model->surfaces().push_back( build_surface( 6, 4, 5 ) );
    model->surfaces().push_back( build_surface( 2, 0, 1 ) );
    model->surfaces().push_back( build_surface( 0, 2, 3 ) );                
    return model;
}

}

//////////////////////////////////////////////////////////////
std::unique_ptr<const Ac3d_file> Ac3d_file_reader::test_cube( float edge_length )
{
    std::unique_ptr<Ac3d_model> model = build_cube( edge_length );
    std::vector<Ac3d_material> materials;
    materials.push_back( Ac3d_material::Color( 1.0f, 1.0f, 1.0f ) );

    return std::make_unique<const Ac3d_file>( std::move(materials), std::move(model) );
}

std::unique_ptr<const Ac3d_file> Ac3d_file_reader::test_cube_group( float edge_length )
{
    std::unique_ptr<Ac3d_model> model = std::make_unique<Ac3d_model>();
    std::unique_ptr<Ac3d_model> child = build_cube( edge_length );
    child->offset() = Math::Local_point( 1.0f, 0, 0 );
    model->push_kid( std::move(child) );
    child = build_cube( edge_length );
    child->offset() = Math::Local_point( 0, 1.0f, 0 );
    model->push_kid( std::move(child) );
    child = build_cube( edge_length );
    child->offset() = Math::Local_point( 0, 0, 1.0f );
    model->push_kid( std::move(child) );

    std::vector<Ac3d_material> materials;
    materials.push_back( Ac3d_material::Color( 1.0f, 1.0f, 1.0f ) );

    return std::make_unique<const Ac3d_file>( std::move(materials), std::move(model) );
}

}}
