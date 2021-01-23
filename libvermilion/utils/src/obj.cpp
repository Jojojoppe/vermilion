#include <vermilion/utils.hpp>

#include <cstdio>
#include <cstring>
#include <fstream>

void __loadObj(std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<Vermilion::Utils::mesh>& meshes, std::vector<std::string>& mtlfiles, const std::string& path){
    struct vertex{
        float x,y,z,w;
    };
    struct normal{
        float i, j, k;
    };
    struct uv{
        float u, v, w;
    };
    struct face{
        unsigned int v1, v2, v3, v4;
        unsigned int n1, n2, n3, n4;
        unsigned int t1, t2, t3, t4;
    };

    std::vector<vertex> vs = {vertex{}};
    std::vector<normal> ns = {normal{}};
    std::vector<uv> us = {uv{}};
    std::vector<face> fs;

    std::ifstream in(path);
    char buf[512];
    size_t lastFaceId = 0;
    size_t totalFaceId = 0;
    bool meshStarted = false;
    std::string materialName;
    while(!in.eof()){
        in.getline(buf, 512);

        if(buf[0]=='#') continue;       // # comment
        if(buf[0]=='o') continue;       // o object name
        if(buf[0]=='s') continue;       // s smooth shading
        if(buf[0]=='g') continue;       // g group name

        if(buf[0]=='m'){                // mtllib material definition
            char * mtlname = buf + strlen("mtllib ");
            mtlfiles.push_back(std::string(mtlname));
        }

        if(buf[0]=='v' && buf[1]==' '){ // v vertex
            vertex v;
            v.w = 1.0f;
            sscanf(buf, "v %f %f %f %f", &v.x, &v.y, &v.z, &v.w);
            vs.push_back(v);
        }

        if(buf[0]=='v' && buf[1]=='n'){ // vn normal
            normal n;
            n.j = 0.0f;
            n.k = 0.0f;
            sscanf(buf, "vn %f %f %f", &n.i, &n.j, &n.k);
            ns.push_back(n);
        }

        if(buf[0]=='v' && buf[1]=='t'){ // vt UV
            uv u;
            u.v = 0.0f;
            u.w = 0.0f;
            sscanf(buf, "vt %f %f %f", &u.u, &u.v, &u.w);
            us.push_back(u);
        }

        if(buf[0]=='f' && buf[1]==' '){ // f face
            face f={};
            sscanf(buf, "f %u/%u/%u %u/%u/%u %u/%u/%u", &f.v1, &f.t1, &f.n1, &f.v2, &f.t2, &f.n2, &f.v3, &f.t3, &f.n3);
            fs.push_back(f);
            totalFaceId++;
        }

        if(buf[0]=='u'){                // usemtl material usage -> Use as grouping
            char * matname = buf + strlen("usemtl ");
            if(!meshStarted){
                materialName = std::string(matname);
                meshStarted = true;
                continue;
            }
            Vermilion::Utils::mesh m = {totalFaceId-lastFaceId, lastFaceId, materialName};
            lastFaceId = totalFaceId;
            meshes.push_back(m);
            materialName = std::string(matname);
        }
    }

    Vermilion::Utils::mesh m = {totalFaceId-lastFaceId, lastFaceId, materialName};
    meshes.push_back(m);

    unsigned int index = 0;
    for(unsigned int i=0; i<fs.size(); i++){
        // First vertex
        vertices.insert(vertices.end(), {vs[fs[i].v1].x, vs[fs[i].v1].y, vs[fs[i].v1].z, vs[fs[i].v1].w});
        if(fs[i].t1)
            vertices.insert(vertices.end(), {us[fs[i].t1].u, us[fs[i].t1].v, us[fs[i].t1].w});
        else
            vertices.insert(vertices.end(), {0, 0, 0});
        vertices.insert(vertices.end(), {ns[fs[i].n1].i, ns[fs[i].n1].j, ns[fs[i].n1].k});

        // Second vertex
        vertices.insert(vertices.end(), {vs[fs[i].v2].x, vs[fs[i].v2].y, vs[fs[i].v2].z, vs[fs[i].v2].w});
        if(fs[i].t2)
            vertices.insert(vertices.end(), {us[fs[i].t2].u, us[fs[i].t2].v, us[fs[i].t2].w});
        else
            vertices.insert(vertices.end(), {0, 0, 0});
        vertices.insert(vertices.end(), {ns[fs[i].n2].i, ns[fs[i].n2].j, ns[fs[i].n2].k});

        // Third vertex
        vertices.insert(vertices.end(), {vs[fs[i].v3].x, vs[fs[i].v3].y, vs[fs[i].v3].z, vs[fs[i].v3].w});
        if(fs[i].t3)
            vertices.insert(vertices.end(), {us[fs[i].t3].u, us[fs[i].t3].v, us[fs[i].t3].w});
        else
            vertices.insert(vertices.end(), {0, 0, 0});
        vertices.insert(vertices.end(), {ns[fs[i].n3].i, ns[fs[i].n3].j, ns[fs[i].n3].k});

        // Indices
        indices.insert(indices.end(), {index, index+1, index+2});
        index+=3;
    }
}

void __loadMtl(std::unordered_map<std::string, Vermilion::Utils::material>& materials, const std::string& path){
    std::ifstream in(path);
    char buf[512];
    std::string currentMaterial;
    bool materialStarted = false;
    Vermilion::Utils::material M;
    while(!in.eof()){
        in.getline(buf, 512);

        if(buf[0]=='#') continue;
        
        if(buf[0]=='n' && buf[1]=='e'){     // newmtl material name
            if(materialStarted){
                materials.insert(std::pair<std::string, Vermilion::Utils::material>(currentMaterial, M));
                M = {};
            }
            materialStarted = true;
            currentMaterial = std::string(buf + strlen("newmtl "));
        }

        if(buf[0]=='m' && buf[1]=='a' && buf[5]=='d'){      // Diffuse map
            std::string texture = std::string(buf + strlen("map_Kd "));
            M.tex_d = texture;
            M.use_tex_d = true;
        }

        if(buf[0]=='K' && buf[1]=='d'){                     // Diffuse color
            sscanf(buf, "Kd %f %f %f", &M.Kdr, &M.Kdg, &M.Kdb);
        }

        if(buf[0]=='K' && buf[1]=='a'){                     // Ambient color
            sscanf(buf, "Ka %f %f %f", &M.Kar, &M.Kag, &M.Kab);
        }

    }
    materials.insert(std::pair<std::string, Vermilion::Utils::material>(currentMaterial, M));
}

void Vermilion::Utils::loadObj(std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<mesh>& meshes, std::unordered_map<std::string, material>& materials, const std::string& objname, const std::string& path){
    std::string objpath = path+"/"+objname+".obj";
    printf("Loading %s ... ", objpath.c_str());

    // Parse obj file
    std::vector<std::string> mtlfiles;
    __loadObj(vertices, indices, meshes, mtlfiles, objpath);

    // Parse mtl files
    for(auto& mfile : mtlfiles){
        std::string mtlfile = path+"/"+mfile;
        __loadMtl(materials, mtlfile);
    }

    // prepend path to all textures
    for(auto& m : materials){
        if(m.second.use_tex_d){
            m.second.tex_d = path+"/"+m.second.tex_d;
        }
    }
    printf("OK\r\n");
}
