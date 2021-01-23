#include <vermilion/utils.hpp>

#include <cstdio>
#include <fstream>

void Vermilion::Utils::loadObj(std::vector<float>& vertices, std::vector<unsigned int>& indices, const std::string& path){
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
        int v1, v2, v3, v4;
        int n1, n2, n3, n4;
        int t1, t2, t3, t4;
    };

    std::vector<vertex> vs = {vertex{}};
    std::vector<normal> ns = {normal{}};
    std::vector<uv> us = {uv{}};
    std::vector<face> fs;

    std::ifstream in(path);
    char buf[512];
    while(!in.eof()){
        in.getline(buf, 512);

        if(buf[0]=='#') continue;       // # comment
        if(buf[0]=='o') continue;       // o object name
        if(buf[0]=='s') continue;       // s smooth shading
        if(buf[0]=='m') continue;       // mtllib material definition
        if(buf[0]=='g') continue;       // g group name
        if(buf[0]=='u') continue;       // usemtl material usage

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
            face f;
            sscanf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &f.v1, &f.t1, &f.n1, &f.v2, &f.t2, &f.n2, &f.v3, &f.t3, &f.n3, &f.v4, &f.t4, &f.n4);
            fs.push_back(f);
        }
    }

    unsigned int index = 0;
    for(int i=0; i<fs.size(); i++){
        // First vertex
        vertices.insert(vertices.end(), {vs[fs[i].v1].x, vs[fs[i].v1].y, vs[fs[i].v1].z, vs[fs[i].v1].w});
        vertices.insert(vertices.end(), {us[fs[i].t1].u, us[fs[i].t1].v, us[fs[i].t1].w});
        vertices.insert(vertices.end(), {ns[fs[i].n1].i, ns[fs[i].n1].j, ns[fs[i].n1].k});
        // Second vertex
        vertices.insert(vertices.end(), {vs[fs[i].v2].x, vs[fs[i].v2].y, vs[fs[i].v2].z, vs[fs[i].v2].w});
        vertices.insert(vertices.end(), {us[fs[i].t2].u, us[fs[i].t2].v, us[fs[i].t2].w});
        vertices.insert(vertices.end(), {ns[fs[i].n2].i, ns[fs[i].n2].j, ns[fs[i].n2].k});
        // Third vertex
        vertices.insert(vertices.end(), {vs[fs[i].v3].x, vs[fs[i].v3].y, vs[fs[i].v3].z, vs[fs[i].v3].w});
        vertices.insert(vertices.end(), {us[fs[i].t3].u, us[fs[i].t3].v, us[fs[i].t3].w});
        vertices.insert(vertices.end(), {ns[fs[i].n3].i, ns[fs[i].n3].j, ns[fs[i].n3].k});

        // Indices
        indices.insert(indices.end(), {index, index+1, index+2});
        index+=3;
    }
}