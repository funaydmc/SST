using namespace app;

// God Mode
DO_APP_FUNC(0x0012106B0, bool, AdventureActor_OnHitActor, (void* hitBox, int uniqueAttackId, int onceAttackTargetCount, void* actor, void* raycastHit, bool damaged, void* hurtEffectPrefab, bool isHittedEffectScale, bool effectIgnoreTimeScale));
DO_APP_FUNC(0x001221D50, bool, AdventureActor_get_damageImmunity, (void* __this));
DO_APP_FUNC(0x001140E00, bool, AdventureTargetBlob_IsPlayerActorAttackable, (void* __this, void* playerActor, int64_t attackRange));

// Only affect UI, actual skill cooldown still applies
DO_APP_FUNC(0x00146AB10, bool, PlayerAdventureActor_CheckSkillReady, (void* __this));
DO_APP_FUNC(0x00146AC00, bool, PlayerAdventureActor_CheckUltimateSkillReady, (void* __this));

DO_APP_FUNC(0x0014715B0, void, PlayerSkillCd_AddEnergy, (void* __this, int energy));
DO_APP_FUNC(0x001472320, void, PlayerSkillCd_ClearEnergy, (void* __this));
DO_APP_FUNC(0x0014730E0, int64_t /*FP*/, PlayerSkillCd_GetEnergy, (void* __this));
DO_APP_FUNC(0x0014749D0, void, PlayerSkillCd_ResetSkillCD, (int skillId));
DO_APP_FUNC(0x001475900, void, PlayerSkillCd_UpdateSkillCD, (void* __this, int64_t /*FP*/ deltaTime));
DO_APP_FUNC(0x001472240, bool, PlayerSkillCd_CanUseCD, (void* __this, int skillId));

// No Cooldown
DO_APP_FUNC(0x001474580, void, PlayerSkillCd_ReduceSkillSection, (void* __this, int skillId, bool beginResume));
DO_APP_FUNC(0x001474CE0, void, PlayerSkillCd_ResetUseInterval, (void* __this, int skillId));

// Unlimited Energy
DO_APP_FUNC(0x00147BC60, void, SkillInfo_get_currentEnergy, (void* __this));
DO_APP_FUNC(0x00147BCE0, void, SkillInfo_set_currentEnergy, (void* __this, int64_t /*FP*/ value));
DO_APP_FUNC(0x00147BCA0, int64_t, SkillInfo_get_totalEnergy, (void* __this));

DO_APP_FUNC(0x0011CB720, void, AdventureSkill_InitSkill, (void* __this, int skillId, void* owner, void* targets, int skillLauncherType, bool isInterruptAttack, bool isBreakable, int skillCastBehaviourType, int countdownTiming, void* finishCallback, void* beginCDForMonsterCallback, bool ingoreCDAndEnergy));