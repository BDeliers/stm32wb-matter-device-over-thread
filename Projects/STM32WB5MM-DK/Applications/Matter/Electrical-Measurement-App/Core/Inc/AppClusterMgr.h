#ifndef APP_CLUSTER_MGR_H
#define APP_CLUSTER_MGR_H

#include <stdbool.h>
#include <stdint.h>

#include <lib/core/DataModelTypes.h>

class AppClusterMgr
{

public:
    static AppClusterMgr& GetInstance(void)
    {
        static AppClusterMgr singleton;
        return singleton;
    }

    void Init(void);

    bool UpdateMatterClusterU32(chip::ClusterId cluster, chip::AttributeId attribute, uint32_t val);

private:
    static const chip::EndpointId ep_id = 1;

};

#endif // APP_CLUSTER_MGR_H
