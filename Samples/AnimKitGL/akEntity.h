/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Xavier T.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/

#ifndef AKENTITY_H
#define AKENTITY_H

#include "akCommon.h"
#include "akTransformState.h"
#include "akAnimationPlayer.h"
#include "akAnimationPlayerSet.h"

class akEntity
{
public:
	akEntity();
	~akEntity();
	
	void draw(void);
	
	void setSkeleton(akSkeleton* skel);
	
	
	void setMesh(akMesh* mesh)
	{
		m_mesh = mesh;
	}
	
	UT_INLINE akTransformState getTransform(void)
	{
		return m_transform;
	}
	
	UT_INLINE void setTransformState(const akTransformState& v)
	{
		m_transform = v;
	}
	
	UT_INLINE akSkeletonPose* getPose(void)
	{
		return m_pose;
	}
	
	UT_INLINE akMesh* getMesh(void)
	{
		return m_mesh;
	}
	
	UT_INLINE akAnimationPlayerSet* getAnimationPlayers(void)
	{
		return &m_players;
	}
	
	UT_INLINE bool isMeshDeformed(void)
	{
		return m_skeleton? true:false;
	}
	
	UT_INLINE bool isPositionAnimated(void)
	{
		return true;
	}
	
	UT_INLINE Matrix4* getPalette(void)
	{
		return m_matrixPalette;
	}

private:
	// object's world transform
	akTransformState m_transform;
	
	// pointers to shared data (resources)
	akMesh*     m_mesh;
	akSkeleton* m_skeleton;
	
	// per object anim data
	akAnimationPlayerSet m_players;
	akSkeletonPose*      m_pose;
	Matrix4*             m_matrixPalette;
};

#endif // AKENTITY_H