#include "CBLT_Animation.hpp"

namespace CBLT {
    //                             speed   overshoot   stiffness  damping
    const AnimationProfile gAnimationProfiles[] = {
        { AnimationEase::NONE,     0.00f,  0.00f,  0.00f,  0.00f },  // NONE             - instant
        { AnimationEase::EASE_OUT, 0.20f,  0.00f,  -0.25,  0.90f },  // EASE              - gentle ease in, p^0.5 // standard
        { AnimationEase::ELASTIC,  0.25f,  0.20f,  0.60f,  3.00f },  // SMOOTH           - tiny overshoot, dies immediately
        { AnimationEase::ELASTIC,  0.12f,  0.80f,  1.00f,  1.20f },  // ELASTIC          - clean overshoot, standard decay
        { AnimationEase::ELASTIC,  0.08f,  1.40f,  1.20f,  0.60f },  // EXTRA_ELASTIC    - big overshoot, slow decay, wiggles
        { AnimationEase::BOUNCE,   0.18f,  1.00f,  1.00f,  1.00f },  // BOUNCE           - standard bounce
        { AnimationEase::LINEAR,   0.25f,  0.00f,  1.00f,  1.00f },  // SNAPPY           - pure linear
        { AnimationEase::EASE_OUT, 0.10f,  0.00f,  1.50f,  1.00f },  // RUBBER           - heavy ease out, p^2.5
        { AnimationEase::ELASTIC,  0.20f,  0.15f,  0.80f,  4.00f },  // STIFF            - barely overshoots, snaps back fast
        { AnimationEase::ELASTIC,  0.06f,  1.80f,  0.70f,  0.15f },  // JELLY            - huge overshoot, oscillates long
        { AnimationEase::BOUNCE,   0.22f,  1.40f,  1.00f,  1.00f },  // BOING            - exaggerated bounce scale
        { AnimationEase::BOUNCE,   0.30f,  0.50f,  1.00f,  1.00f },  // TAP              - subtle quick bounce
        { AnimationEase::EASE_IN,  0.08f,  0.00f,  1.50f,  1.00f },  // SLUGGISH         - very slow, heavy ease in p^2.5
        { AnimationEase::LINEAR,   0.50f,  0.00f,  1.00f,  1.00f },  // INSTANT          - fast linear, near snap
        { AnimationEase::ELASTIC,  0.10f,  1.0f,   0.50f,  2.50f },  // OVERSHOOT_SMOOTH - soft overshoot, low freq, dies fast
    };

    Animator::Animator(void) : tx(0.0f), ty(0.0f), init(false), t(AnimationType::EASE), interp() {}

    Animator::~Animator(void) {}

    void Animator::SetType(AnimationType type) {
        t = type;
    }

    void Animator::MoveTo(UT::f32 toX, UT::f32 toY) {
        if (t == AnimationType::NONE || !init) {
            tx = toX;
            ty = toY;
            init = true;
            return;
        }

        if (tx == toX && ty == toY) return;

        const AnimationProfile& profile = gAnimationProfiles[static_cast<UT::i32>(t)];
        interp.Start(tx, ty, toX, toY, profile.speed);
    }

    void Animator::Update(void) {
        if (!interp.IsActive()) return;

        interp.Update();

        if (!interp.IsActive()) return;

        const AnimationProfile& profile = gAnimationProfiles[static_cast<UT::i32>(t)];
        UT::f32 p = interp.Progress();
        UT::f32 easedT = p;

        switch (profile.ease) {
            case AnimationEase::NONE:     easedT = 1.0f; break;
            case AnimationEase::LINEAR:
                easedT = p * profile.stiffness + (p * p) * (1.0f - profile.stiffness); break;
            case AnimationEase::EASE_IN:
                easedT = std::pow(p, 1.0f + profile.stiffness); break;
            case AnimationEase::EASE_OUT:
                easedT = 1.0f - std::pow(1.0f - p, 1.0f + profile.stiffness); break;
            case AnimationEase::ELASTIC: {
                const UT::f32 c4 = (2.0f * 3.14159f) / 3.0f;
                if      (p == 0.0f) easedT = 0.0f;
                else if (p == 1.0f) easedT = 1.0f;
                else {
                    UT::f32 decay     = std::pow(2.0f, -10.0f * p * profile.damping);
                    UT::f32 wave      = std::sin((p * 10.0f * profile.stiffness - 0.75f) * c4);
                    UT::f32 amplitude = 1.0f + profile.overshoot; // How far past target it goes
                    easedT = 1.0f - decay * wave * amplitude;
                }
                break;
            }
            case AnimationEase::BOUNCE: {
                UT::f32 b = p;
                UT::f32 scale = profile.overshoot;
                if      (b < 1.0f / 2.75f) { easedT = scale * 7.5625f * b * b; }
                else if (b < 2.0f / 2.75f) { b -= 1.5f   / 2.75f; easedT = scale * (7.5625f * b * b + 0.75f);    }
                else if (b < 2.5f / 2.75f) { b -= 2.25f  / 2.75f; easedT = scale * (7.5625f * b * b + 0.9375f);  }
                else                        { b -= 2.625f / 2.75f; easedT = scale * (7.5625f * b * b + 0.984375f);}
                break;
            }
        }

        tx = interp.SourceX() + (interp.TargetX() - interp.SourceX()) * easedT;
        ty = interp.SourceY() + (interp.TargetY() - interp.SourceY()) * easedT;
    }

    UT::b Animator::IsMoving(void) const {
        return interp.IsActive();
    }

    void Animator::Snap(UT::f32 toX, UT::f32 toY) {
        tx = toX;
        ty = toY;
        init = true;
    }

    AnimationProfile ReadAnimationFile(const std::string& path) {
        std::string resourcePath = ".";
    
        #if defined(__linux__)
            if (const char* p = getenv("CBLT_RESOURCES"))
                resourcePath = p;    
        
            resourcePath += '/';
        #endif
    
        AnimationProfile ap;
    
        std::ifstream f(
            resourcePath +
            "options/anim/" +
            path +
            ".conf"
        );
    
        // std::cout << resourcePath + "options/anim/" + path + ".conf\n";
        
        if (!f.is_open()) {
            UE::Log(
                "Failed to open animation file: " +
                path
            );
        }
    
        auto trim = [](std::string s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
            return s;
        };
    
        auto ease = [](const std::string& v) {
            if (v == "LINEAR")   return AnimationEase::LINEAR;
            if (v == "EASE_OUT") return AnimationEase::EASE_OUT;
            if (v == "EASE_IN")  return AnimationEase::EASE_IN;
            if (v == "ELASTIC")  return AnimationEase::ELASTIC;
            if (v == "BOUNCE")   return AnimationEase::BOUNCE;
    
            return AnimationEase::NONE;
        };
    
        std::string line;
        bool block = false;
    
        while (std::getline(f, line)) {
            line = trim(line);
    
            if (
                line.empty() ||
                line[0] == '!'
            )
                continue;
    
            if (line == "%anim") {
                block = !block;
                continue;
            }
    
            if (!block)
                continue;
    
            size_t p = line.find(':');
    
            if (p == std::string::npos)
                continue;
    
            std::string k = trim(
                line.substr(0, p)
            );
    
            std::string v = trim(
                line.substr(p + 1)
            );
    
            if      (k == "ease")       ap.ease = ease(v);
            else if (k == "speed")      ap.speed = std::stof(v);
            else if (k == "overshoot")  ap.overshoot = std::stof(v);
            else if (k == "stiffness")  ap.stiffness = std::stof(v);
            else if (k == "damping")    ap.damping = std::stof(v);
        }
    
        return ap;
    }
} // CBLT