#include "InputLoader.h"
#include <fstream>

class Algo
{
public:
    Algo(InputLoader& input) : 
        loader(input),
        requests(input.requests),
        caches(input.caches),
        videos(input.videos),
        endpoints(input.endpoints),
        ofs("outputs2.txt")
    {}

    void Sort()
    {
        for (Request& r : requests)
        {
            EndPoint& e = endpoints[r.endpoint];
            int Ld = e.data_center_lantency;
            int minL = Ld;
            for (int i = 0; i < e.cache_latencies.size();i++)
            {
                Cache& c = caches[e.cache_ids[i]];
                if (cacheAvailableSpace(videos, c) > videos[r.video].size
                 && e.cache_latencies[i] < minL)
                {
                    minL = e.cache_latencies[i];
                }
            }
            r.points = r.request_nb * (Ld - minL);
        }

        sort(requests.begin(), requests.end(), [](const Request& r1, const Request& r2)
                {
                    return r1.points > r2.points;
                });
    }

    void Run()
    {
        vector<int> rs;
        for (int i = 0; i < requests.size(); i++)
        {
            rs.push_back(i);
        }
        while (true)
        {
            if (requests.size() != 0) 
            {
                if (requests.size() % 100 == 0) cout << requests.size() << endl;
                Sort();
                Request& r = requests[0];
                int cid = findAvailableCache(r);
                if (cid != -1)
                {
                    caches[cid].videos.insert(r.video);
                }
                auto it = requests.begin();;
                while (it !=requests.end())
                {
                    if ((*it).id == r.id) break;
                    ++it;
                }
                requests.erase(it);
            }
            else break;
        }
        cout << "Start to fill" << endl;

        for (int i = 0; i < caches.size() ; i++)
        {
            Cache& c = caches[i];
            if (c.videos.size() > 0)
            {
                ofs << i << " ";
                for (auto it = c.videos.begin(); it != c.videos.end(); ++it)
                {
                    ofs << (*it) << " ";
                }
                ofs << endl;
            }
        }
        ofs.close();
    }

    int findAvailableCache(Request& r)
    {
        EndPoint& e = endpoints[r.endpoint];
        for (int i = 0 ; i < e.cache_latencies.size(); i++)
        {
            int cid = e.cache_ids[i];
            if (cacheAvailableSpace(loader.videos, caches[cid]) > videos[r.video].size)
            {
                return cid; 
            }
        }
        return -1;
    }

    vector<Cache>& caches;
    vector<Request>& requests;
    vector<Video>& videos;
    vector<EndPoint>& endpoints;
    
    ofstream ofs;
    InputLoader& loader;
};
