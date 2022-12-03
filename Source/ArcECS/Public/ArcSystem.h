// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArcSystemInternal.h"

struct ARCECS_API FArcSystemConfig
{
public:

	TArray<FName> Labels;

	TArray<FName> AfterLabels;
	TArray<FName> BeforeLabels;

public:

	FArcSystemConfig() = default;
	FArcSystemConfig(const FName& Label);
	
	FArcSystemConfig& WithLabel(const FName& Label);
	FArcSystemConfig& After(const FName& Label);
	FArcSystemConfig& Before(const FName& Label);
};

struct ARCECS_API FArcSystemBuilder
{
public:
	
    TSharedPtr<FArcSystemInternalBase> SystemInternal;

	/** The first label is the system name and must be unique within a stage. */
	TArray<FName> Labels;
	
	TArray<FName> AfterLabels;
	TArray<FName> BeforeLabels;

public:

	FArcSystemBuilder& WithLabel(const FName& Label);
	FArcSystemBuilder& After(const FName& Label);
	FArcSystemBuilder& Before(const FName& Label);

	FName GetName() const;
};

template<typename... Args>
FArcSystemBuilder ArcSystem(void(*Function)(Args...))
{
	const FName Name = FName(FGuid::NewGuid().ToString(EGuidFormats::Short));
	
	FArcSystemBuilder SystemBuilder;
	SystemBuilder.Labels.Add(Name);
	
	FArcSystemInternal<Args...> System;
	System.Function = Function;
	
	SystemBuilder.SystemInternal = MakeShared<FArcSystemInternal<Args...>>(System);
	return SystemBuilder;
}

template<typename... Args>
FArcSystemBuilder ArcSystem(const FName& Name, void(*Function)(Args...))
{
	FArcSystemBuilder SystemBuilder;
	SystemBuilder.Labels.Add(Name);
	
	FArcSystemInternal<Args...> System;
	System.Function = Function;
	
	SystemBuilder.SystemInternal = MakeShared<FArcSystemInternal<Args...>>(System);
	return SystemBuilder;
}


struct ARCECS_API FArcSystemSet
{
public:
	
	TArray<FArcSystemBuilder> Systems;
	
	TArray<FName> Labels;
	TArray<FName> AfterLabels;
	TArray<FName> BeforeLabels;

private:

	FName CurrentSystem = NAME_None;
	
public:
	
	FArcSystemSet& WithLabel(const FName& Label);
	FArcSystemSet& After(const FName& Label);
	FArcSystemSet& Before(const FName& Label);
	
	FArcSystemSet& AddSystem(const FArcSystemBuilder& System);
	FArcSystemSet& AddSystemSeq(const FArcSystemBuilder& System);
	
	template<typename... Args>
	FArcSystemSet& AddSystem(void(*Function)(Args...))
	{
		return AddSystem(ArcSystem(Function));
	}
	
	template<typename... Args>
	FArcSystemSet& AddSystem(void(*Function)(Args...), const FArcSystemConfig& Config)
	{
		FArcSystemBuilder SystemBuilder = ArcSystem(Function);
		if (Config.Labels.Num() > 0)
		{
			SystemBuilder.Labels = Config.Labels;
		}
		SystemBuilder.AfterLabels = Config.AfterLabels;
		SystemBuilder.BeforeLabels = Config.BeforeLabels;
		return AddSystem(SystemBuilder);
	}
	
	template<typename... Args>
	FArcSystemSet& AddSystemSeq(void(*Function)(Args...))
	{
		return AddSystemSeq(ArcSystem(Function));
	}
	
	template<typename... Args>
	FArcSystemSet& AddSystemSeq(void(*Function)(Args...), const FArcSystemConfig& Config)
	{
		FArcSystemBuilder SystemBuilder = ArcSystem(Function);
		if (Config.Labels.Num() > 0)
		{
			SystemBuilder.Labels = Config.Labels;
		}
		SystemBuilder.AfterLabels = Config.AfterLabels;
		SystemBuilder.BeforeLabels = Config.BeforeLabels;
		return AddSystemSeq(SystemBuilder);
	}
};
