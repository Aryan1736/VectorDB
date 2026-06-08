#include "HNSW.hpp"

#include <algorithm>
#include <cmath>

HNSW::HNSW(
    DistFn dist,
    int m,
    int efBuild)
    :
    M(m),
    M0(2 * m),
    ef_build(efBuild),
    mL(1.0f / std::log((float)m)),
    distFn(dist),
    rng(42)
{
}

int HNSW::randLevel()
{
    std::uniform_real_distribution<float> u(0.0f, 1.0f);

    return
        (int)std::floor(
            -std::log(u(rng)) * mL
        );
}

std::vector<std::pair<float,int>>
HNSW::searchLayer(
    const std::vector<float>& q,
    int ep,
    int ef,
    int lyr)
{
    std::unordered_map<int,bool> vis;

    std::priority_queue<
        std::pair<float,int>,
        std::vector<std::pair<float,int>>,
        std::greater<>
    > cands;

    std::priority_queue<
        std::pair<float,int>
    > found;

    float d0 =
        distFn(
            q,
            G[ep].item.emb
        );

    vis[ep] = true;

    cands.push({d0, ep});
    found.push({d0, ep});

    while(!cands.empty())
    {
        auto [cd, cid] =
            cands.top();

        cands.pop();

        if(
            (int)found.size() >= ef
            &&
            cd > found.top().first
        )
            break;

        if(
            lyr >=
            (int)G[cid].nbrs.size()
        )
            continue;

        for(int nid : G[cid].nbrs[lyr])
        {
            if(vis[nid] || !G.count(nid))
                continue;

            vis[nid] = true;

            float nd =
                distFn(
                    q,
                    G[nid].item.emb
                );

            if(
                (int)found.size() < ef
                ||
                nd < found.top().first
            )
            {
                cands.push({nd, nid});

                found.push({nd, nid});

                if(
                    (int)found.size() > ef
                )
                    found.pop();
            }
        }
    }

    std::vector<std::pair<float,int>> res;

    while(!found.empty())
    {
        res.push_back(found.top());
        found.pop();
    }

    std::sort(
        res.begin(),
        res.end()
    );

    return res;
}

std::vector<int>
HNSW::selectNbrs(
    std::vector<std::pair<float,int>>& cands,
    int maxM)
{
    std::vector<int> r;

    for(
        int i = 0;
        i < std::min(
            (int)cands.size(),
            maxM
        );
        i++
    )
    {
        r.push_back(
            cands[i].second
        );
    }

    return r;
}

void HNSW::insert(
    const VectorItem& item)
{
    int id =
        item.id;

    int lvl =
        randLevel();

    G[id] =
    {
        item,
        lvl,
        std::vector<
            std::vector<int>
        >(lvl + 1)
    };

    if(entryPt == -1)
    {
        entryPt = id;
        topLayer = lvl;
        return;
    }

    int ep =
        entryPt;

    for(
        int lc = topLayer;
        lc > lvl;
        lc--
    )
    {
        if(
            lc <
            (int)G[ep].nbrs.size()
        )
        {
            auto W =
                searchLayer(
                    item.emb,
                    ep,
                    1,
                    lc
                );

            if(!W.empty())
                ep = W[0].second;
        }
    }

    for(
        int lc =
        std::min(topLayer, lvl);
        lc >= 0;
        lc--
    )
    {
        auto W =
            searchLayer(
                item.emb,
                ep,
                ef_build,
                lc
            );

        int maxM =
            (lc == 0)
            ? M0
            : M;

        auto sel =
            selectNbrs(
                W,
                maxM
            );

        G[id].nbrs[lc] =
            sel;

        for(int nid : sel)
        {
            if(!G.count(nid))
                continue;

            if(
                (int)G[nid].nbrs.size()
                <= lc
            )
            {
                G[nid].nbrs.resize(
                    lc + 1
                );
            }

            auto& conn =
                G[nid].nbrs[lc];

            conn.push_back(id);

            if(
                (int)conn.size()
                > maxM
            )
            {
                std::vector<
                    std::pair<float,int>
                > ds;

                for(int c : conn)
                {
                    if(G.count(c))
                    {
                        ds.push_back(
                        {
                            distFn(
                                G[nid].item.emb,
                                G[c].item.emb
                            ),
                            c
                        });
                    }
                }

                std::sort(
                    ds.begin(),
                    ds.end()
                );

                conn.clear();

                for(
                    int i = 0;
                    i < maxM
                    &&
                    i < (int)ds.size();
                    i++
                )
                {
                    conn.push_back(
                        ds[i].second
                    );
                }
            }
        }

        if(!W.empty())
            ep = W[0].second;
    }

    if(lvl > topLayer)
    {
        topLayer = lvl;
        entryPt = id;
    }
}

std::vector<std::pair<float,int>>
HNSW::knn(
    const std::vector<float>& q,
    int k,
    int ef)
{
    if(entryPt == -1)
        return {};

    int ep =
        entryPt;

    for(
        int lc = topLayer;
        lc > 0;
        lc--
    )
    {
        if(
            lc <
            (int)G[ep].nbrs.size()
        )
        {
            auto W =
                searchLayer(
                    q,
                    ep,
                    1,
                    lc
                );

            if(!W.empty())
                ep = W[0].second;
        }
    }

    auto W =
        searchLayer(
            q,
            ep,
            std::max(ef, k),
            0
        );

    if((int)W.size() > k)
        W.resize(k);

    return W;
}

void HNSW::remove(int id)
{
    if(!G.count(id))
        return;

    for(auto& [nid, nd] : G)
    {
        for(auto& layer : nd.nbrs)
        {
            layer.erase(
                std::remove(
                    layer.begin(),
                    layer.end(),
                    id
                ),
                layer.end()
            );
        }
    }

    G.erase(id);
}

size_t HNSW::size() const
{
    return G.size();
}