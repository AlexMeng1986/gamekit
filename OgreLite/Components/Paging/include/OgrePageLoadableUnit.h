/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef __Ogre_PageLoadableUnit_H__
#define __Ogre_PageLoadableUnit_H__

#include "OgrePagingPrerequisites.h"
#include "OgreAtomicWrappers.h"

namespace Ogre
{
	/** \addtogroup Optional Components
	*  @{
	*/
	/** \addtogroup Paging
	*  Some details on paging component
	*  @{
	*/

	/** Define the interface for an element that is loadable. 
	@remarks
		Loadable units are able to load and unload certain parts in a background thread.
		However, we assume that the render system is not threadsafe in itself, and
		partition up the tasks so that those that need to be performed in the 
		main render thread (which is generally those that access GPU resources)
		are clearly identified. The sequence of loading is:

		<ol><li>Construct (main render thread): do the minimum to construct an instance</li>
		<li>Prepare (background thread): pull data from disk, generate data on the CPU, process. No
			GPU resources may be accessed here. </li>
		<li>Load (main render thread): create / modify GPU resources</li>
		</ol>

		The sequence of unloading is:
		<ol>
		<li>Unload (main render thread): destroy GPU resources</li>
		<li>Unprepare (background thread): free up CPU resources or otherwise perform shutdown
			tasks that do not access the GPU.</li>
		<li>Destroy (main render thread): physical deletion</li>		
		</ol>

		Subclasses should implement the protected prepareImpl() et al methods rather
		than the main public methods. 
	*/
	class _OgrePagingExport PageLoadableUnit : public PageAlloc
	{
	public:
		PageLoadableUnit();
		virtual ~PageLoadableUnit();

		/** Status of the unit. 
		*/
		enum UnitStatus
		{
			/// Just defined, not loaded
			STATUS_UNLOADED,
			/// In the process of getting data from a stream, or generating it (background thread)
			STATUS_PREPARING, 
			/** At this stage all data has been read, and all non-GPU tasks have been done. 
			This is the end of the background thread's involvement.
			*/
			STATUS_PREPARED, 
			/// Finalising the load in the main render thread
			STATUS_LOADING,
			/// Data loaded / generated 
			STATUS_LOADED, 
			/// Unloading in main render thread (goes back to STATUS_PREPARED)
			STATUS_UNLOADING,
			/// Unpreparing, potentially in a background thread (goes back to STATUS_UNLOADED)
			STATUS_UNPREPARING
		};

		/// Returns the current status
		virtual UnitStatus getStatus() const { return mStatus.get(); }

		/** Returns true if the unit has been fully loaded, false otherwise.
		*/
		virtual bool isLoaded() const { return (mStatus.get() == STATUS_LOADED); }

		/** Returns whether the unit is currently in the process of
		loading.
		*/
		virtual bool isLoading() const
		{
			UnitStatus s = mStatus.get();
			return (s == STATUS_LOADING || s == STATUS_PREPARING);
		}

		/** Read data from a serialiser & prepare.
		@remarks
			'prepare' means to pull data in from a file, and to do as much processing
			on it as required to be ready to create GPU resources. Since this method can 
			be called from a non-render thread, this implementation must not access
			any GPU resources.
		@returns true if data was loaded, false otherwise
		*/
		virtual bool prepare(StreamSerialiser& stream);
		/** Finalise the loading of the data.
		@remarks
			This implementation will finalise any work done in prepare() and create
			or access any GPU resources. This method will be called from the main
			render thread.
		*/
		virtual void load();

		/** Unload the unit, deallocating any GPU resources. 
		@remarks
			This method will be called in the main render thread just before the unprepare()
			call (which may be done in the background). Any GPU-dependent 
			instances must be cleaned up in this call, anything else can be done
			in the unprepare() call.
		*/
		virtual void unload();

		/** Deallocate any background resources.
		@remarks
			This method may be called in a background, non-render thread after 
			unload() therefore should only deallocate non-GPU resources. 
			GPU resources should be freed in unload(). 

		*/
		virtual void unprepare();


		/** Set this unit's status to STATUS_LOADED without going through the 
			load sequence. 
		@remarks
			Usually you would do this because you're manually defining the content
			rather than loading it, some other way than via PageProvider. 
		*/
		virtual void setLoaded();
		/** Manually change a loadable unit's status - advanced use only.
		@remarks Takes old & new status to avoid race conditions
		@returns Whether the status change was successful
		*/
		virtual bool _changeStatus(UnitStatus oldStatus, UnitStatus newStatus);



	protected:
		AtomicScalar<UnitStatus> mStatus;

		/// Should be overridden by subclasses to implement 'prepare' action
		virtual bool prepareImpl(StreamSerialiser& stream) = 0;
		/// Should be overridden by subclasses to implement 'load' action
		virtual void loadImpl() = 0;
		/// Should be overridden by subclasses to implement 'unload' action
		virtual void unloadImpl() = 0;
		/// Should be overridden by subclasses to implement 'unprepare' action
		virtual void unprepareImpl() = 0;

		/// Internal method, must be called by subclass destructors
		void destroy();
	};

	/** @} */
	/** @} */
}

#endif
