// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArcSystemInternal.h"

struct ARCECS_API FArcSystemBuilder
{
public:
	
    TSharedPtr<FArcSystemInternalBase> SystemInternal;

	/** The first label is the system name and must be unique within a stage. */
	TArray<FName> Labels;
	
	TArray<FName> AfterLabels;
	TArray<FName> BeforeLabels;

public:

	FArcSystemBuilder& WithLabel(const FName& InLabel);
	FArcSystemBuilder& After(const FName& Label);
	FArcSystemBuilder& Before(const FName& Label);

	FName GetName() const;
};

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
};
