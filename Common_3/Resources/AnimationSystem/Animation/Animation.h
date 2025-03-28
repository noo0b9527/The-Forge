/*
 * Copyright (c) 2017-2025 The Forge Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#pragma once

#include "../ThirdParty/OpenSource/ozz-animation/include/ozz/animation/runtime/blending_job.h"

#include "../../../Utilities/Math/MathTypes.h"

#include "Clip.h"
#include "ClipController.h"
#include "ClipMask.h"
#include "Rig.h"

// Maximum number of clips that can make up one animation
const uint32_t MAX_NUM_CLIPS = 10;

// Descriptors for each layer/clip that will make up the blended animation
struct LayerProperty
{
    Clip*           mClip = NULL;
    ClipController* mClipController = NULL;
    ClipMask*       mClipMask = NULL;
    bool            mAdditive = false;
};

// Properties that define how the clips will be blended when mAutoSetBlendParams is true
enum BlendType
{
    EQUAL,               // Each animation will have equal influence
    CROSS_DISSOLVE,      // The animations will fade into one another in the order they were added
    CROSS_DISSOLVE_SYNC, // The animations will fade into one another in the order they were added, syncronizing their speeds as they fade
                         // into eachother
    MANUAL               // The user is always responsible for setting all the animations weights and playback speeds
};

// User will have to predefine to pass into Animation's intialize function
struct AnimationDesc
{
    Rig*          mRig = NULL;
    uint32_t      mNumLayers = 0;
    LayerProperty mLayerProperties[MAX_NUM_CLIPS];
    BlendType     mBlendType = BlendType::EQUAL;
};

// Allows for blending and sampling of loaded clips
class FORGE_API Animation
{
public:
    // Set up an animation for a rig based on the animation description
    void Initialize(AnimationDesc animationDesc);

    // Needs to be called if initialize was called
    void Exit();

    // Will sample the animation at dt, storing the local transform results in localTrans
    bool Sample(float dt, ozz::span<SoaTransform>& localTrans);

    // Hard set the current animation with timeRatio [0,1] (based on the longest clip)
    void SetTimeRatio(float timeRatio);

private:
    // Sets the various blend parameters based on the type of blend set
    void UpdateBlendParameters();

    // Blend the sampled clips together based on their blend parameters
    bool Blend(ozz::span<SoaTransform>& localTrans);

public:
    // Pointer to the rig that this animation corresponds to
    Rig* mRig = NULL;

    // Array of objects responsible for managing clip specifc sample and blend properties
    ClipController* mClipControllers[MAX_NUM_CLIPS] = {};

    // Array of clips that make up the animation
    Clip* mClips[MAX_NUM_CLIPS] = {};

    // Array of clip masks that can alter per joint weights of clips
    ClipMask* mClipMasks[MAX_NUM_CLIPS] = {};

    // Sampling cache that will be given as input when each clip is sampled
    ozz::animation::SamplingJob::Context* mClipSamplingCaches[MAX_NUM_CLIPS] = {};

    // The buffer of local transforms that will be updated as output when each clip is sampled
    ozz::span<SoaTransform> mClipLocalTrans[MAX_NUM_CLIPS];

    // The blend layers that will be set each sampling based on each clip's properties
    ozz::span<ozz::animation::BlendingJob::Layer> mLayers;
    ozz::span<ozz::animation::BlendingJob::Layer> mAdditiveLayers;

    // Number of clips that make up this animation
    uint32_t mNumClips = 0;

    // Number of clips that are additive
    uint32_t mNumAdditiveClips = 0;

    // Index in mClips that gives the longest clip
    uint32_t mLongestClipIndex = 0;

    // Current timeRatio [0,1] of the animation
    float mTimeRatio = 0.f;

    // Length of the longest clip that makes up the animation
    float mDuration = 0.f;

    // The job blends the bind pose to the output when the accumulated weight of
    // all layers is less than this threshold value.
    // Must be greater than 0.f.
    // Set to Ozz's default min value
    float mThreshold = ozz::animation::BlendingJob().threshold;

    // Type of blend that defines how the clips blend parameters will be managed
    BlendType mBlendType = BlendType::EQUAL;

    // Blend ratio for CROSS_DISSOLVE and CROSS_DISSOLVE_SYNC in range [0,1] that
    // controls all blend parameters and synchronizes playback speeds. A value of
    // 0 gives full weight to the first animation, and 1 to the last.
    // Initialized in the middle to represet the maximum amount of clips
    float mBlendRatio = 0.5f;

    // Controls if the UpdateBlendParameters() function gets called or not
    // A value of false implies that all blend parameters will be set externally
    bool mAutoSetBlendParams = true;
};