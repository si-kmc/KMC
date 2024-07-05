#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "vec3.h"



class KMCLogger
{
private:
	
	struct EventLog {
		int64_t step;
		int64_t src_site_id;
		int64_t dst_site_id;
		vec3d position;
		int particle_id;

		EventLog() {};
		EventLog(int64_t step_, int64_t src_site_id_, int64_t dst_site_id_, const vec3d& position_, int particle_id_) :
			step(step_), src_site_id(src_site_id_), dst_site_id(dst_site_id_), 
			position(position_),
			particle_id(particle_id_)
		{
		};
	};

	std::vector<EventLog> m_event_log;


public:
	
	void Initialize(FILE* fp, double* box_axis_org)
	{
		if (fp) {
			fprintf(fp, "BOX");
			for (int i = 0; i < 12; ++i) {
				fprintf(fp, "\t%lf", box_axis_org[i]);
			}
			fprintf(fp, "\n");
			/*
			for (int64_t i = 0; i < num_sites; ++i) {
				if (sites[i].existance) {
					const vec3d& position = sites[i].Position();
					fprintf(fp, "0\tKMC\tH\t%d\t%lf\t%lf\t%lf\n", m_sites[i].existance, position.x, position.y, position.z);
				}
			}*/

		}

	}

	void Add(int64_t step, int64_t src_site_id_, int64_t dst_site_id_, const vec3d& position_, int particle_id_) {
		m_event_log.push_back(EventLog(step, src_site_id_, dst_site_id_, position_, particle_id_));
	}

	void Flush(FILE* fp)
	{

		if (fp) {
			const auto it_end = m_event_log.end();
			for (auto it = m_event_log.begin(); it != it_end; ++it) {

				if (it->dst_site_id < 0) {
					if (it->src_site_id >= 0) {
						fprintf(fp, "%I64d\tDEL\tW\t%d\t%lf\t%lf\t%lf\n", it->step, it->particle_id, 0.0, 0.0, 0.0);
					}
				} else {
					if (it->src_site_id < 0) {
						fprintf(fp, "%I64d\tBCA\tW\t%d\t%lf\t%lf\t%lf\n", it->step, it->particle_id, it->position.x, it->position.y, it->position.z);
					//	fprintf(fp, "%I64d\tKMC\tH\t%d\t%lf\t%lf\t%lf\n", it->step, it->particle_id, it->position.x, it->position.y, it->position.z);
					}else{
						fprintf(fp, "%I64d\tKMC\tW\t%d\t%lf\t%lf\t%lf\n", it->step, it->particle_id, it->position.x, it->position.y, it->position.z);
					}
				}

			}
		}
		m_event_log.clear();
	}
	
};
