// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class FArcEntityHandle
{
private:

	uint16 Index = MAX_uint16;
	uint16 Generation = MAX_uint16;

public:

	FArcEntityHandle() = default;
	FArcEntityHandle(uint16 InIndex, uint16 InGeneration) : Index(InIndex), Generation(InGeneration) {}

	uint16 GetIndex() const { return Index; }
	uint16 GetGeneration() const { return Generation; }

	bool IsValid(const class FArcWorld& World) const;
	
	template<typename T>
	bool HasComponent(const class FArcWorld& World) const;
	
	template<typename T>
	T* GetComponent(const class FArcWorld& World) const;
};
