#include "AppClusterMgr.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <platform/CHIPDeviceLayer.h>

void AppClusterMgr::Init(void)
{

}

bool AppClusterMgr::UpdateMatterClusterU32(chip::ClusterId cluster, chip::AttributeId attribute, uint32_t val)
{
    using namespace chip::app::Clusters;
    using namespace ElectricalMeasurement::Attributes;
    using namespace EnedisTic::Attributes;

    if ((cluster != ElectricalMeasurement::Id) && (cluster != EnedisTic::Id))
    {
        return false;
    }

    EmberAfStatus ret = EMBER_ZCL_STATUS_FAILURE;

    chip::DeviceLayer::PlatformMgr().LockChipStack();

    if (cluster == ElectricalMeasurement::Id)
    {
        switch (attribute)
        {
        case ApparentPower::Id: { ret = ApparentPower::Set(ep_id, val); break; }
        case RmsCurrent::Id:    { ret = RmsCurrent::Set(ep_id, val);    break; }
        case RmsVoltage::Id:    { ret = RmsVoltage::Set(ep_id, val);    break; }
        default:                { break; }
        }
    }
    else if (cluster == EnedisTic::Id)
    {
        switch (attribute)
        {
        case Adsc::Id:       { ret = Adsc::Set(ep_id, val);         break; }
        case Vtic::Id:       { ret = Vtic::Set(ep_id, val);         break; }
        //case Ngtf::Id:       { ret = Ngtf::Set(ep_id, val);         break; }
        //case Ltarf::Id:      { ret = Ltarf::Set(ep_id, val);        break; }
        case East::Id:       { ret = East::Set(ep_id, val);         break; }
        case Easf01::Id:     { ret = Easf01::Set(ep_id, val);       break; }
        case Easf02::Id:     { ret = Easf02::Set(ep_id, val);       break; }
        case Easf03::Id:     { ret = Easf03::Set(ep_id, val);       break; }
        case Easf04::Id:     { ret = Easf04::Set(ep_id, val);       break; }
        case Easf05::Id:     { ret = Easf05::Set(ep_id, val);       break; }
        case Easf06::Id:     { ret = Easf06::Set(ep_id, val);       break; }
        case Easf07::Id:     { ret = Easf07::Set(ep_id, val);       break; }
        case Easf08::Id:     { ret = Easf08::Set(ep_id, val);       break; }
        case Easf09::Id:     { ret = Easf09::Set(ep_id, val);       break; }
        case Easf10::Id:     { ret = Easf10::Set(ep_id, val);       break; }
        case Easd01::Id:     { ret = Easd01::Set(ep_id, val);       break; }
        case Easd02::Id:     { ret = Easd02::Set(ep_id, val);       break; }
        case Easd03::Id:     { ret = Easd03::Set(ep_id, val);       break; }
        case Easd04::Id:     { ret = Easd04::Set(ep_id, val);       break; }
        case Eait::Id:       { ret = Eait::Set(ep_id, val);         break; }
        case Erq1::Id:       { ret = Erq1::Set(ep_id, val);         break; }
        case Erq2::Id:       { ret = Erq2::Set(ep_id, val);         break; }
        case Erq3::Id:       { ret = Erq3::Set(ep_id, val);         break; }
        case Erq4::Id:       { ret = Erq4::Set(ep_id, val);         break; }
        case Irms1::Id:      { ret = Irms1::Set(ep_id, val);        break; }
        case Irms2::Id:      { ret = Irms2::Set(ep_id, val);        break; }
        case Irms3::Id:      { ret = Irms3::Set(ep_id, val);        break; }
        case Urms1::Id:      { ret = Urms1::Set(ep_id, val);        break; }
        case Urms2::Id:      { ret = Urms2::Set(ep_id, val);        break; }
        case Urms3::Id:      { ret = Urms3::Set(ep_id, val);        break; }
        case Pref::Id:       { ret = Pref::Set(ep_id, val);         break; }
        case Pcoup::Id:      { ret = Pcoup::Set(ep_id, val);        break; }
        case Sinsts::Id:     { ret = Sinsts::Set(ep_id, val);       break; }
        case Sinsts1::Id:    { ret = Sinsts1::Set(ep_id, val);      break; }
        case Sinsts2::Id:    { ret = Sinsts2::Set(ep_id, val);      break; }
        case Sinsts3::Id:    { ret = Sinsts3::Set(ep_id, val);      break; }
        case Smaxsn::Id:     { ret = Smaxsn::Set(ep_id, val);       break; }
        case Smaxsn1::Id:    { ret = Smaxsn1::Set(ep_id, val);      break; }
        case Smaxsn2::Id:    { ret = Smaxsn2::Set(ep_id, val);      break; }
        case Smaxsn3::Id:    { ret = Smaxsn3::Set(ep_id, val);      break; }
        case SmaxsnM1::Id:   { ret = SmaxsnM1::Set(ep_id, val);     break; }
        case Smaxsn1M1::Id:  { ret = Smaxsn1M1::Set(ep_id, val);    break; }
        case Smaxsn2M1::Id:  { ret = Smaxsn2M1::Set(ep_id, val);    break; }
        case Smaxsn3M1::Id:  { ret = Smaxsn3M1::Set(ep_id, val);    break; }
        case Sinscti::Id:    { ret = Sinscti::Set(ep_id, val);      break; }
        case Smaxin::Id:     { ret = Smaxin::Set(ep_id, val);       break; }
        case SmaxinM1::Id:   { ret = SmaxinM1::Set(ep_id, val);     break; }
        case Ccasn::Id:      { ret = Ccasn::Set(ep_id, val);        break; }
        case CcasnM1::Id:    { ret = CcasnM1::Set(ep_id, val);      break; }
        case Ccain::Id:      { ret = Ccain::Set(ep_id, val);        break; }
        case CcainM1::Id:    { ret = CcainM1::Set(ep_id, val);      break; }
        case Umoy1::Id:      { ret = Umoy1::Set(ep_id, val);        break; }
        case Umoy2::Id:      { ret = Umoy2::Set(ep_id, val);        break; }
        case Umoy3::Id:      { ret = Umoy3::Set(ep_id, val);        break; }
        case Stge::Id:       { ret = Stge::Set(ep_id, val);         break; }
        case Dpm1::Id:       { ret = Dpm1::Set(ep_id, val);         break; }
        case Fpm1::Id:       { ret = Fpm1::Set(ep_id, val);         break; }
        case Dpm2::Id:       { ret = Dpm2::Set(ep_id, val);         break; }
        case Fpm2::Id:       { ret = Fpm2::Set(ep_id, val);         break; }
        case Dpm3::Id:       { ret = Dpm3::Set(ep_id, val);         break; }
        case Fpm3::Id:       { ret = Fpm3::Set(ep_id, val);         break; }
        //case Msg1::Id:       { ret = Msg1::Set(ep_id, val);         break; }
        //case Msg2::Id:       { ret = Msg2::Set(ep_id, val);         break; }
        case Prm::Id:        { ret = Prm::Set(ep_id, val);          break; }
        case Relais::Id:     { ret = Relais::Set(ep_id, val);       break; }
        case Ntarf::Id:      { ret = Ntarf::Set(ep_id, val);        break; }
        case Njourf::Id:     { ret = Njourf::Set(ep_id, val);       break; }
        case NjourfP1::Id:   { ret = NjourfP1::Set(ep_id, val);     break; }
        //case PjourfP1::Id:   { ret = PjourfP1::Set(ep_id, val);     break; }
        //case Ppointe::Id:    { ret = Ppointe::Set(ep_id, val);      break; }
        default:             { break; }
        }
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    return (ret == EMBER_ZCL_STATUS_SUCCESS);
}
