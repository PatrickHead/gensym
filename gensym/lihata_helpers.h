/*
 *  Copyright 2026 - Patrick Head
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 *  @file lihata_helpers.h
 *  @brief helper functions for creating lihata document
 */

#ifndef LIHATA_HELPERS_H
#define LIHATA_HELPERS_H

void lihata_add_line(lht_node_t *node,
                     char *stroke,
                     int x1,
                     int y1,
                     int x2,
                     int y2);
void lihata_add_arc(lht_node_t *node,
                    int oid,
                    char *stroke,
                    int cx,
                    int cy,
                    int r,
                    double sang,
                    double dang,
                    bool svalid,
                    int sx,
                    int sy,
                    bool evalid,
                    int ex,
                    int ey);
void lihata_add_uuid(lht_node_t *node);

#endif // LIHATA_HELPERS_H
