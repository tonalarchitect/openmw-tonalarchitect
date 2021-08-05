#ifndef OPENMW_COMPONENTS_DETOURNAVIGATOR_ASYNCNAVMESHUPDATER_H
#define OPENMW_COMPONENTS_DETOURNAVIGATOR_ASYNCNAVMESHUPDATER_H

#include "navmeshcacheitem.hpp"
#include "offmeshconnectionsmanager.hpp"
#include "tilecachedrecastmeshmanager.hpp"
#include "tileposition.hpp"
#include "navmeshtilescache.hpp"
#include "waitconditiontype.hpp"

#include <osg/Vec3f>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <deque>
#include <set>
#include <thread>
#include <tuple>
#include <list>

class dtNavMesh;

namespace Loading
{
    class Listener;
}

namespace DetourNavigator
{
    enum class ChangeType
    {
        remove = 0,
        mixed = 1,
        add = 2,
        update = 3,
    };

    inline std::ostream& operator <<(std::ostream& stream, ChangeType value)
    {
        switch (value) {
            case ChangeType::remove:
                return stream << "ChangeType::remove";
            case ChangeType::mixed:
                return stream << "ChangeType::mixed";
            case ChangeType::add:
                return stream << "ChangeType::add";
            case ChangeType::update:
                return stream << "ChangeType::update";
        }
        return stream << "ChangeType::" << static_cast<int>(value);
    }

    struct Job
    {
        osg::Vec3f mAgentHalfExtents;
        std::weak_ptr<GuardedNavMeshCacheItem> mNavMeshCacheItem;
        TilePosition mChangedTile;
        unsigned mTryNumber;
        ChangeType mChangeType;
        int mDistanceToPlayer;
        int mDistanceToOrigin;
        std::chrono::steady_clock::time_point mProcessTime;
    };

    using JobIt = std::list<Job>::iterator;

    class AsyncNavMeshUpdater
    {
    public:
        AsyncNavMeshUpdater(const Settings& settings, TileCachedRecastMeshManager& recastMeshManager,
            OffMeshConnectionsManager& offMeshConnectionsManager);
        ~AsyncNavMeshUpdater();

        void post(const osg::Vec3f& agentHalfExtents, const SharedNavMeshCacheItem& mNavMeshCacheItem,
            const TilePosition& playerTile, const std::map<TilePosition, ChangeType>& changedTiles);

        void wait(Loading::Listener& listener, WaitConditionType waitConditionType);

        void reportStats(unsigned int frameNumber, osg::Stats& stats) const;

    private:
        std::reference_wrapper<const Settings> mSettings;
        std::reference_wrapper<TileCachedRecastMeshManager> mRecastMeshManager;
        std::reference_wrapper<OffMeshConnectionsManager> mOffMeshConnectionsManager;
        std::atomic_bool mShouldStop;
        mutable std::mutex mMutex;
        std::condition_variable mHasJob;
        std::condition_variable mDone;
        std::condition_variable mProcessed;
        std::list<Job> mJobs;
        std::deque<JobIt> mWaiting;
        std::map<osg::Vec3f, std::set<TilePosition>> mPushed;
        Misc::ScopeGuarded<TilePosition> mPlayerTile;
        NavMeshTilesCache mNavMeshTilesCache;
        Misc::ScopeGuarded<std::map<osg::Vec3f, std::map<TilePosition, std::thread::id>>> mProcessingTiles;
        std::map<osg::Vec3f, std::map<TilePosition, std::chrono::steady_clock::time_point>> mLastUpdates;
        std::set<std::tuple<osg::Vec3f, TilePosition>> mPresentTiles;
        std::map<std::thread::id, std::deque<JobIt>> mThreadsQueues;
        std::vector<std::thread> mThreads;

        void process() noexcept;

        bool processJob(const Job& job);

        JobIt getNextJob();

        JobIt getJob(std::deque<JobIt>& jobs, bool changeLastUpdate);

        void postThreadJob(JobIt job, std::deque<JobIt>& queue);

        void writeDebugFiles(const Job& job, const RecastMesh* recastMesh) const;

        void repost(JobIt job);

        std::thread::id lockTile(const osg::Vec3f& agentHalfExtents, const TilePosition& changedTile);

        void unlockTile(const osg::Vec3f& agentHalfExtents, const TilePosition& changedTile);

        inline std::size_t getTotalJobs() const;

        void cleanupLastUpdates();

        int waitUntilJobsDoneForNotPresentTiles(const std::size_t initialJobsLeft, std::size_t& maxJobsLeft, Loading::Listener& listener);

        void waitUntilAllJobsDone();

        inline void removeJob(JobIt job);
    };
}

#endif
