#ifndef CFGMGR_H
#define CFGMGR_H

#include "cfgid.h"

class QVariant;
/*
配置管理类。
提供获取配置和更新配置的接口。
*/
class CFGMgr
{
public:
	CFGMgr(void);
	virtual ~CFGMgr(void);

public:
	static CFGMgr* Instance();
	static void ReleaseInstance();

	void GetConfig(quint32 id, QVariant& vVal);
	void UpdateConfig(quint32 id, const QVariant& vVal);

private:
	void InitSettings();
	void WriteSetting(const QString& key, const QVariant& vVal);
	void ReadSettings(const QString& key, QVariant& vVal);

private:
	static CFGMgr* m_pInst;
};

#endif // CFGMGR_H

