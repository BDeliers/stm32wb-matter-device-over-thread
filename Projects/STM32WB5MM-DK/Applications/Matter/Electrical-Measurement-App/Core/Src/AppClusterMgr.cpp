#include "AppClusterMgr.h"

#include <app-common/zap-generated/attributes/Accessors.h>
#include <platform/CHIPDeviceLayer.h>

void AppClusterMgr::Init(void)
{

}

bool AppClusterMgr::UpdateMatterCluster(chip::ClusterId cluster, chip::AttributeId attribute, const void* val_ptr, uint8_t val_size)
{
    using namespace chip::app::Clusters;
    using namespace ElectricalMeasurement::Attributes;
    using namespace EnedisTic::Attributes;

    if ((cluster != ElectricalMeasurement::Id) && (cluster != EnedisTic::Id)
        || val_ptr == nullptr || val_size < 1)
    {
        return false;
    }

    EmberAfStatus ret = EMBER_ZCL_STATUS_FAILURE;

    if (chip::DeviceLayer::PlatformMgr().TryLockChipStack())
    {
        uint64_t tmp = 0;
        if (val_size <= sizeof(uint64_t))
        {
            memcpy(&tmp, val_ptr, val_size);
        }
        const char* tmp_char_ptr = static_cast<const char*>(val_ptr);

        if (cluster == ElectricalMeasurement::Id)
        {
            switch (attribute)
            {
            case ApparentPower::Id: { ret = ApparentPower::Set(ep_id, static_cast<ApparentPower::TypeInfo::Type>(tmp)); break; }
            case RmsCurrent::Id:    { ret = RmsCurrent::Set(ep_id, static_cast<RmsCurrent::TypeInfo::Type>(tmp));       break; }
            case RmsVoltage::Id:    { ret = RmsVoltage::Set(ep_id, static_cast<RmsVoltage::TypeInfo::Type>(tmp));       break; }
            default:                { break; }
            }
        }
        else if (cluster == EnedisTic::Id)
        {
            switch (attribute)
            {
            case Adsc::Id:       { ret = Adsc::Set(ep_id, static_cast<Adsc::TypeInfo::Type>(tmp));                  break; }
            case Vtic::Id:       { ret = Vtic::Set(ep_id, static_cast<Vtic::TypeInfo::Type>(tmp));                  break; }
            // DATE is (intentionnaly) not defined in the Matter cluster
            case Ngtf::Id:       { ret = Ngtf::Set(ep_id, chip::CharSpan(tmp_char_ptr, strlen(tmp_char_ptr)));      break; }
            case Ltarf::Id:      { ret = Ltarf::Set(ep_id, chip::CharSpan(tmp_char_ptr, strlen(tmp_char_ptr)));     break; }
            case East::Id:       { ret = East::Set(ep_id, static_cast<East::TypeInfo::Type>(tmp));                  break; }
            case Easf01::Id:     { ret = Easf01::Set(ep_id, static_cast<Easf01::TypeInfo::Type>(tmp));              break; }
            case Easf02::Id:     { ret = Easf02::Set(ep_id, static_cast<Easf02::TypeInfo::Type>(tmp));              break; }
            case Easf03::Id:     { ret = Easf03::Set(ep_id, static_cast<Easf03::TypeInfo::Type>(tmp));              break; }
            case Easf04::Id:     { ret = Easf04::Set(ep_id, static_cast<Easf04::TypeInfo::Type>(tmp));              break; }
            case Easf05::Id:     { ret = Easf05::Set(ep_id, static_cast<Easf05::TypeInfo::Type>(tmp));              break; }
            case Easf06::Id:     { ret = Easf06::Set(ep_id, static_cast<Easf06::TypeInfo::Type>(tmp));              break; }
            case Easf07::Id:     { ret = Easf07::Set(ep_id, static_cast<Easf07::TypeInfo::Type>(tmp));              break; }
            case Easf08::Id:     { ret = Easf08::Set(ep_id, static_cast<Easf08::TypeInfo::Type>(tmp));              break; }
            case Easf09::Id:     { ret = Easf09::Set(ep_id, static_cast<Easf09::TypeInfo::Type>(tmp));              break; }
            case Easf10::Id:     { ret = Easf10::Set(ep_id, static_cast<Easf10::TypeInfo::Type>(tmp));              break; }
            case Easd01::Id:     { ret = Easd01::Set(ep_id, static_cast<Easd01::TypeInfo::Type>(tmp));              break; }
            case Easd02::Id:     { ret = Easd02::Set(ep_id, static_cast<Easd02::TypeInfo::Type>(tmp));              break; }
            case Easd03::Id:     { ret = Easd03::Set(ep_id, static_cast<Easd03::TypeInfo::Type>(tmp));              break; }
            case Easd04::Id:     { ret = Easd04::Set(ep_id, static_cast<Easd04::TypeInfo::Type>(tmp));              break; }
            case Eait::Id:       { ret = Eait::Set(ep_id, static_cast<Eait::TypeInfo::Type>(tmp));                  break; }
            case Erq1::Id:       { ret = Erq1::Set(ep_id, static_cast<Erq1::TypeInfo::Type>(tmp));                  break; }
            case Erq2::Id:       { ret = Erq2::Set(ep_id, static_cast<Erq2::TypeInfo::Type>(tmp));                  break; }
            case Erq3::Id:       { ret = Erq3::Set(ep_id, static_cast<Erq3::TypeInfo::Type>(tmp));                  break; }
            case Erq4::Id:       { ret = Erq4::Set(ep_id, static_cast<Erq4::TypeInfo::Type>(tmp));                  break; }
            case Irms1::Id:      { ret = Irms1::Set(ep_id, static_cast<Irms1::TypeInfo::Type>(tmp));                break; }
            case Irms2::Id:      { ret = Irms2::Set(ep_id, static_cast<Irms2::TypeInfo::Type>(tmp));                break; }
            case Irms3::Id:      { ret = Irms3::Set(ep_id, static_cast<Irms3::TypeInfo::Type>(tmp));                break; }
            case Urms1::Id:      { ret = Urms1::Set(ep_id, static_cast<Urms1::TypeInfo::Type>(tmp));                break; }
            case Urms2::Id:      { ret = Urms2::Set(ep_id, static_cast<Urms2::TypeInfo::Type>(tmp));                break; }
            case Urms3::Id:      { ret = Urms3::Set(ep_id, static_cast<Urms3::TypeInfo::Type>(tmp));                break; }
            case Pref::Id:       { ret = Pref::Set(ep_id, static_cast<Pref::TypeInfo::Type>(tmp));                  break; }
            case Pcoup::Id:      { ret = Pcoup::Set(ep_id, static_cast<Pcoup::TypeInfo::Type>(tmp));                break; }
            case Sinsts::Id:     { ret = Sinsts::Set(ep_id, static_cast<Sinsts::TypeInfo::Type>(tmp));              break; }
            case Sinsts1::Id:    { ret = Sinsts1::Set(ep_id, static_cast<Sinsts1::TypeInfo::Type>(tmp));            break; }
            case Sinsts2::Id:    { ret = Sinsts2::Set(ep_id, static_cast<Sinsts2::TypeInfo::Type>(tmp));            break; }
            case Sinsts3::Id:    { ret = Sinsts3::Set(ep_id, static_cast<Sinsts3::TypeInfo::Type>(tmp));            break; }
            case Smaxsn::Id:     { ret = Smaxsn::Set(ep_id, static_cast<Smaxsn::TypeInfo::Type>(tmp));              break; }
            case Smaxsn1::Id:    { ret = Smaxsn1::Set(ep_id, static_cast<Smaxsn1::TypeInfo::Type>(tmp));            break; }
            case Smaxsn2::Id:    { ret = Smaxsn2::Set(ep_id, static_cast<Smaxsn2::TypeInfo::Type>(tmp));            break; }
            case Smaxsn3::Id:    { ret = Smaxsn3::Set(ep_id, static_cast<Smaxsn3::TypeInfo::Type>(tmp));            break; }
            case SmaxsnM1::Id:   { ret = SmaxsnM1::Set(ep_id, static_cast<SmaxsnM1::TypeInfo::Type>(tmp));          break; }
            case Smaxsn1M1::Id:  { ret = Smaxsn1M1::Set(ep_id, static_cast<Smaxsn1M1::TypeInfo::Type>(tmp));        break; }
            case Smaxsn2M1::Id:  { ret = Smaxsn2M1::Set(ep_id, static_cast<Smaxsn2M1::TypeInfo::Type>(tmp));        break; }
            case Smaxsn3M1::Id:  { ret = Smaxsn3M1::Set(ep_id, static_cast<Smaxsn3M1::TypeInfo::Type>(tmp));        break; }
            case Sinscti::Id:    { ret = Sinscti::Set(ep_id, static_cast<Sinscti::TypeInfo::Type>(tmp));            break; }
            case Smaxin::Id:     { ret = Smaxin::Set(ep_id, static_cast<Smaxin::TypeInfo::Type>(tmp));              break; }
            case SmaxinM1::Id:   { ret = SmaxinM1::Set(ep_id, static_cast<SmaxinM1::TypeInfo::Type>(tmp));          break; }
            case Ccasn::Id:      { ret = Ccasn::Set(ep_id, static_cast<Ccasn::TypeInfo::Type>(tmp));                break; }
            case CcasnM1::Id:    { ret = CcasnM1::Set(ep_id, static_cast<CcasnM1::TypeInfo::Type>(tmp));            break; }
            case Ccain::Id:      { ret = Ccain::Set(ep_id, static_cast<Ccain::TypeInfo::Type>(tmp));                break; }
            case CcainM1::Id:    { ret = CcainM1::Set(ep_id, static_cast<CcainM1::TypeInfo::Type>(tmp));            break; }
            case Umoy1::Id:      { ret = Umoy1::Set(ep_id, static_cast<Umoy1::TypeInfo::Type>(tmp));                break; }
            case Umoy2::Id:      { ret = Umoy2::Set(ep_id, static_cast<Umoy2::TypeInfo::Type>(tmp));                break; }
            case Umoy3::Id:      { ret = Umoy3::Set(ep_id, static_cast<Umoy3::TypeInfo::Type>(tmp));                break; }
            case Stge::Id:       { ret = Stge::Set(ep_id, static_cast<Stge::TypeInfo::Type>(tmp));                  break; }
            case Dpm1::Id:       { ret = Dpm1::Set(ep_id, static_cast<Dpm1::TypeInfo::Type>(tmp));                  break; }
            case Fpm1::Id:       { ret = Fpm1::Set(ep_id, static_cast<Fpm1::TypeInfo::Type>(tmp));                  break; }
            case Dpm2::Id:       { ret = Dpm2::Set(ep_id, static_cast<Dpm2::TypeInfo::Type>(tmp));                  break; }
            case Fpm2::Id:       { ret = Fpm2::Set(ep_id, static_cast<Fpm2::TypeInfo::Type>(tmp));                  break; }
            case Dpm3::Id:       { ret = Dpm3::Set(ep_id, static_cast<Dpm3::TypeInfo::Type>(tmp));                  break; }
            case Fpm3::Id:       { ret = Fpm3::Set(ep_id, static_cast<Fpm3::TypeInfo::Type>(tmp));                  break; }
            case Msg1::Id:       { ret = Msg1::Set(ep_id, chip::CharSpan(tmp_char_ptr, strlen(tmp_char_ptr)));      break; }
            case Msg2::Id:       { ret = Msg2::Set(ep_id, chip::CharSpan(tmp_char_ptr, strlen(tmp_char_ptr)));      break; }
            case Prm::Id:        { ret = Prm::Set(ep_id, static_cast<Prm::TypeInfo::Type>(tmp));                    break; }
            case Relais::Id:     { ret = Relais::Set(ep_id, static_cast<Relais::TypeInfo::Type>(tmp));              break; }
            case Ntarf::Id:      { ret = Ntarf::Set(ep_id, static_cast<Ntarf::TypeInfo::Type>(tmp));                break; }
            case Njourf::Id:     { ret = Njourf::Set(ep_id, static_cast<Njourf::TypeInfo::Type>(tmp));              break; }
            case NjourfP1::Id:   { ret = NjourfP1::Set(ep_id, static_cast<NjourfP1::TypeInfo::Type>(tmp));          break; }
            case PjourfP1::Id:   { ret = PjourfP1::Set(ep_id, chip::CharSpan(tmp_char_ptr, strlen(tmp_char_ptr)));  break; }
            case Ppointe::Id:    { ret = Ppointe::Set(ep_id, chip::CharSpan(tmp_char_ptr, strlen(tmp_char_ptr)));   break; }
            default:             { break; }
            }
        }

        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    }

    return (ret == EMBER_ZCL_STATUS_SUCCESS);
}
