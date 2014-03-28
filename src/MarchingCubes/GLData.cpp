#include "GLData.h"
#include <iostream>
#include <fstream>
#include "Grid.h"

#include <bilLib/binfile.h>
#include <bilLib/commonfunctions.h>

//-----------------------------------------------------------------------------
GLData::GLData():
m_hyperMinLimits(437500,106392),
m_hyperMaxLimits(438532,109590)
{
}


//-----------------------------------------------------------------------------
GLData::GLData(const GLData &i_glData)
{
   setGLData(i_glData);
}

//-----------------------------------------------------------------------------
void GLData::setGLData(const GLData &i_glData)
{
   m_indices.resize(i_glData.m_indices.size());
   for(unsigned int i=0; i<i_glData.m_indices.size(); ++i)
   {
      m_indices[i] = i_glData.m_indices[i];
   }
   m_normals.resize(i_glData.m_normals.size());
   for(unsigned int i=0; i<i_glData.m_normals.size(); ++i)
   {
      m_normals[i] = i_glData.m_normals[i];
   }
   m_vertices.resize(i_glData.m_vertices.size());
   for(unsigned int i=0; i<i_glData.m_vertices.size(); ++i)
   {
      m_vertices[i] = i_glData.m_vertices[i];
   }
   m_UVs.resize(i_glData.m_UVs.size());
   for(unsigned int i=0; i<i_glData.m_UVs.size(); ++i)
   {
      m_UVs[i] = i_glData.m_UVs[i];
   }
}

//-----------------------------------------------------------------------------
void GLData::createUVsBIL(const std::string &i_bilFilename)
{
   if(i_bilFilename=="")
   {
       return;
   }
   try
   {
      bilLib::BinFile file(i_bilFilename);
      std::string map_info = file.FromHeader("map info");

      unsigned int nsamps=bilLib::StringToUINT(file.FromHeader("samples"));
      unsigned int nlines=bilLib::StringToUINT(file.FromHeader("lines"));

      ngl::Vec2 min(atof(bilLib::GetItemFromString(map_info,3,',').c_str()),
                     atof(bilLib::GetItemFromString(map_info,4,',').c_str())-
                    nlines*atof(bilLib::GetItemFromString(map_info,6,',').c_str()));
      ngl::Vec2 max(min.m_x+nsamps*atof(bilLib::GetItemFromString(map_info,5,',').c_str()),
                    atof(bilLib::GetItemFromString(map_info,4,',').c_str()));
      std::cout << min.m_x << " " << min.m_y << "\n";
      std::cout << max.m_x << " " << max.m_y << "\n";
      file.Close();
      m_UVs.resize(m_vertices.size()/3*2+1);
      for(unsigned int i=0; i<m_vertices.size()/3; ++i)
      {
         m_UVs[i*2  ] = (m_vertices[i*3  ]-min.m_x)/(max.m_x-min.m_x);
         m_UVs[i*2+1] = (m_vertices[i*3+1]-min.m_y)/(max.m_y-min.m_y);
      }
   }
   catch(bilLib::BinaryReader::BRexception e)
   {
      std::cout<<e.what()<<std::endl;
      std::cout<<e.info<<std::endl;
   }

}

//-----------------------------------------------------------------------------
void GLData::createUVsIGM(const std::string &i_igmFile)
{
   if(i_igmFile=="")
   {
      return;
   }
   m_UVs.resize(m_vertices.size()/3*2+1);
   Grid *grid = new Grid(i_igmFile,30);
   for(unsigned int i=0; i<m_vertices.size()/3; ++i)
   {
      const ngl::Vec2 nextPixelPos =
               grid->getPixelPositionScaled0_1(m_vertices[i*3],m_vertices[i*3+1]);
      m_UVs[i*2  ] = nextPixelPos.m_x;
      m_UVs[i*2+1] = nextPixelPos.m_y;
   }
   delete grid;
}


//-----------------------------------------------------------------------------
unsigned int GLData::addVertex(const ngl::Vec3 &i_vertex)
{
   m_vertices.push_back(i_vertex.m_x);
   m_vertices.push_back(i_vertex.m_y);
   m_vertices.push_back(i_vertex.m_z);

   return (m_vertices.size()/3-1);
}

//-----------------------------------------------------------------------------
ngl::Vec3 GLData::getVertex(unsigned int i_index)
{
    return ngl::Vec3(m_vertices[i_index/3  ],
                     m_vertices[i_index/3+1],
                     m_vertices[i_index/3+2]);
}

//-----------------------------------------------------------------------------
void GLData::exportToObj(std::string i_name)const
{
    std::ofstream myfile;
    const std::string extension = ".obj";
    if (i_name.find(extension)!=std::string::npos)
    {
      std::size_t pos = i_name.find_last_of(extension);
      if (pos!=i_name.size()-1)
      {
        i_name+=extension;
      }
    }
    else
    {
      i_name+=extension;
    }
    myfile.open(i_name.c_str());
      if (myfile.is_open())
      {
        int vsize = m_vertices.size();
        int nsize = m_normals.size();
        int isize = m_indices.size();
        for(int i=0; i<vsize; i+=3)
        {
          myfile <<"v "<<m_vertices[i]<<" "<< m_vertices[i+1]
                 << " "<<m_vertices[i+2] << "\n";
        }
        myfile <<"\n\n";
        for(int i=0; i<nsize; i+=3)
        {
          myfile <<"vn "<< m_normals[i]<<" "<< m_normals[i+1]
                 << " "<< m_normals[i+2] << "\n";
        }
        myfile <<"\n\n";
        for(int i=0; i<isize; i+=3)
        {
          int x = (int)m_indices[i];
          int y = (int)m_indices[i+1];
          int z = (int)m_indices[i+2];
          if(x!=y && y!=z && x!=z)
          {
            x++;y++;z++;
            myfile << "f "<< x <<
                    "//" << x <<
                    " " << y <<
                    "//" << y <<
                    " " << z <<
                    "//" << z <<
                    "\n";
          }
        }
        myfile.close();
      }
      std::cout << "File saved\n";
}



//-----------------------------------------------------------------------------
GLData::~GLData()
{
}
