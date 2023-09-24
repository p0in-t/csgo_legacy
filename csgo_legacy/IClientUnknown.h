#pragma once
#include "Includes.h"
#include "IHandleEntity.h"
#include "ICollideable.h"

class IClientNetworkable;
class C_BaseEntity;
class IClientRenderable;
class IClientEntity;
class IClientThinkable;
class IClientModelRenderable;
class IClientAlphaProperty;

abstract_class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable * GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual C_BaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty() = 0;
};