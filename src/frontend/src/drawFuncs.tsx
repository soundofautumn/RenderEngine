import { client } from "./client";
import { IPoint } from "./types";

class DrawFunc {
  public readonly requiredPointers: number;
  public readonly apiEndpoint: string;
  public color: string = "#ffffffff";
  public algorithm: number = 0;

  constructor(requiredPointers: number, apiEndpoint: string) {
    this.requiredPointers = requiredPointers;
    this.apiEndpoint = apiEndpoint;
  }

  public draw(pointers: IPoint[]): Promise<void> {
    return new Promise((resolve, reject) => {
      if (pointers.length < this.requiredPointers) {
        reject("Not enough pointers");
      }
      client("/engine/draw", {
        data: {
          [this.apiEndpoint]: {
            ...Object.fromEntries(pointers.map((pointer, index) => {
              return ['p' + (index + 1), {
                ...pointer,
              }];
            })),
            color: {
              r: parseInt(this.color.slice(1, 3), 16),
              g: parseInt(this.color.slice(3, 5), 16),
              b: parseInt(this.color.slice(5, 7), 16),
              a: parseInt(this.color.slice(7, 9), 16),
            },
            algorithm: this.algorithm,
          }
        }
      }).then(() => {
        resolve();
      }).catch(e => {
        console.error(e);
        reject("Failed to draw");
      })
    });
  }
}

export const drawLine = new DrawFunc(2, "Line");